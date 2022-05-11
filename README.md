# Sift
Sift provides a novel two-tier methodology that combines the power of refinement with the ability to automate proofs. Sift decomposes the proofs of complex distributed implementations into a number of refinement steps, each of which is amenable to automation. This repo shows how we use Sift to prove the refinement of a number of complex systems with little manual effort. 

## Dependencies
Install Python2.7 and pip.

Install [IC3PO](https://github.com/aman-goel/ic3po) and [Ivy](https://github.com/GLaDOS-Michigan/ivy/tree/sift) by running `bash build.sh`.

After installing IC3PO, add `$(ic3po_path)/pysmt` to `PYTHONPATH` so that ic3po can use its customized pysmt for SMT solving.

## Verifying the systems
1. Startint from the spec and the implementation, we can generate the transition system in VMT format so that IC3PO can check.
    ```
    python2 ic3po/ivy/ivy/ivy_to_vmt.py isolate=system leader/leader_system.ivy leader.vmt
    ```
    `isolate=system` specifies that we're translating the refinement between the system (implementation) layer and it's upper layer. We only need to pass the lowest layer, as ivy can find this layer from the included files.

2. Use `ic3po`  to generate invariants.
    ```
    python2 ic3po/ic3po.py -g univ -v 2 -n $(test_name) leader.vmt
    ```

    `ic3po` would ask for the initial size of the instance. You can check our choise in `run.sh` under each experiments.
   * When `ic3po` finds a counter example, it would report an violation and shows the execution trace at the end of `output/$(test_name)/$(test_name).log`.
   * When `ic3po` fins an inductive invariant for this vmt file, `output/$(test_name)/$(test_name).inv` shows it.
   * Some detailed information can be found in `output/$(test_name)/$(test_name).result`. `sz-invariant` shows the total number invariants in this inductive invariant; `wall-time-sec` and `memory-mb` shows the time and memory usage.
  
3. Remove given properties and converting operator and constants back in `inv.txt`.
    ```
    python2 ../converter.py output/$(test_name)/$(test_name).inv > leader.inv
    ```

4. Paste the `leader.inv` back into the isolate with other invariants. (e.g. [Line 12 in leader_system.ivy](https://github.com/GLaDOS-Michigan/Sift/blob/master/leader/leader_system.ivy#L27)).

    After adding the inductive invariants for all refinements, run `ivy_check leader_system.ivy` to verify the whole system.

5. `bash tests.sh` generates invariants for all examples.

## Building the systems
1. You can use `ivyc leader_system.ivy` to generate the C++ implementation for this system, and compile the binary code.

2. To run the system, use `./leader_system $(num_of_nodes) $(my_idn) $(my_index)`. For example, we can run a system with 2 nodes by `./leader_system 2 5 0`; `./leader_system 2 8 1`. You should be able to see the debug log of all sending and receiving messages, and `node1` is elected.

## Performance evaluation
See [perf_eval](https://github.com/GLaDOS-Michigan/Sift/tree/master/perf_eval) for more details about building/running the systems.
