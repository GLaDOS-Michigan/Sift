echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=protocol1 raft_system.ivy rprotocol1.vmt
python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system raft_system.ivy rsystem.vmt

echo "running ic3po"

echo -e "3\n 1\n 1\n 1\n 3\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n rprotocol1 rprotocol1.vmt
echo -e "3\n 1\n 1\n 1\n 1\n 1\n 1\n 3\n 2\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n rsystem rsystem.vmt

echo "converting inductive invariant"

python2 ../converter.py output/rprotocol1/rprotocol1.inv > rprotocol1.inv
python2 ../converter.py output/rsystem/rsystem.inv > rsystem.inv
