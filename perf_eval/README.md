# Sift Eval Documentation
This README outlines how to compile and run the SHT, MultiPaxos, Raft, Toy Lock, 2PC, and Leader Election systems described in Sift.

---
## Sift

### Build and Compile

To run these scripts, follow the dependency instructions in the parent directory and make sure to have c++ version >= 11

##### Scripts
This directory contains 3 scripts:

- `build.sh`: compiles systems from ivy to C++, and compiles the systems to executables. 
- `clean.sh`: removes any generated C++ and generated executables 
- `configure_dist.sh`: helps to configure the systems to be run distributed 

---

run `./build.sh` in this directory to convert SHT, MultiPaxos, and Raft systems into C++ and compile them to an executable. This will compile the ivy to C++, copy the `.cpp` and `.h` files into the respective sub-directories in the perf_eval directory, and compile them to executables. This script will also compile the clients for each of the systems (located in their respective sub-directories)

Running `./build.sh` also builds the Toy Lock, 2PC, and Leader Election system into executables. There are no accompanying clients for these systems, but they can still be run independently. 

run `./clean.sh` to remove all generated `.cpp`, `.h`, client and system executables.  

##### Manually building and compiling
To genterate an ivy system into C++ (generates `.cpp` and `.h`): 

`ivy_to_cpp target=repl isolate=iso_impl [name]_system.ivy`

to compile the generated systems, and clients:

`g++ -std=c++11 -o [name]_server [name]_system.cpp -pthread`

`g++ -std=c++11 -o [name]_client [name]_client.cpp -pthread`

##### Configure 
Configure ip address of different machines and experiment with specific details in the specific system's `config.h` file.

**NOTE**: The config file is identical between systems, but is kept separate to make different configurations across systems possible. The config should be identical across all machines being used (if running a system in a distributed manner). The default in each config file is set for localhost. 

To help reduce duplication of configuring, make use of the `configure_dist.sh` script. Running this script will copy the entire perf_eval directory to the list of identified machines.
To run the `configure_dist.sh`:

`./configure_dist.sh [list of ip's and locations]`

- [list of ip's and locations]: is a list of remote hosts to copy perf_eval to and the specified location. eg. `./configure_dist.sh username@127.0.0.1:[location]`

### Running the Systems/Clients

**To run the server(s):**

`./[name]_server [num of nodes] [id]`

- [num of nodes]: specifies how many replicas will be part of the system.
- [id]: which id this node is, 0 indexed. The ids should be in the following range; `0 <= id < [num of nodes]`

These arguments correspond to what is configured in `config.h`

**NOTE**: This ^ generally holds true. See the [System Specfics](#System Specifics) section for system-specific instructions. ie. SHT_server only takes id as input argument

**To run the client(s):**

Configure the IP addresses of the client and nodes in config.h. The ip-config is a map, where key -1 is reserved for the location of client threads, the servers are indexed by their key, 0,1,2....etc. The client process creates _n_ concurrent client threads that are configurable in config.h by changing `num_threads`

To run the client use the following command:

`./[name]_client [num of nodes]`

- [num of nodes]: specifies how many replicas will be part of the system.

The client will record and print out data at the end of the experiment to the terminal. 

**Note:** make sure to start the server nodes before the clients. Starting the server nodes in the id order works best. 

---

Note: the standard ivy-generated C++ will not natively support nodes hosted on separate machines with different ip's or responding to an external client. The included `.cpp` and `.h` files have been modified only to include this additional attribute. (i.e `void get_addr_dyno()`, `writeToFRONTEND()`) This is the purpose of the `.patch` files in some sub-directories.

---
#### System Specifics
##### Leader Election
Leader election takes an additional argument to run.

`./leader_system [node.size] [idn] [me]`

##### Two-Phase Commit
2pc does not run automatically but can be run interactively using the following commands

`system.server.prepare`

`system.server.commit`

`system.server.abort`

##### SHT
SHT only takes a single argument to run:

`./[name]_server [id]` 

There is a built-in script to delegate ranges of keys across 3 servers. After starting each server:

- `./sht_server 0`
- `./sht_server 1`
- `./sht_server 2`

You can run the following command on `sht_server 0` : `system.server.delegate_(42,1)` which will have delegate keys 333-999 to server 1. Then running `system.server.delegate_(42,2)` on `sht_server 1` to delegate keys 666-999 to server 2.

