echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=protocol1 paxos_system.ivy pprotocol1.vmt
python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=protocol2 paxos_system.ivy pprotocol2.vmt
python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=protocol3 paxos_system.ivy pprotocol3.vmt
python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system paxos_system.ivy psystem.vmt

echo "running ic3po"
echo -e "3\n 1\n 1\n 1\n 3\n 1\n 1\n 1\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n pprotocol1 pprotocol1.vmt
echo -e "1\n 1\n 3\n 1\n 1\n 1\n 3\n 1\n 1\n 1\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n pprotocol2 pprotocol2.vmt
echo -e "1\n 1\n 3\n 1\n 1\n 1\n 3\n 1\n 1\n 1\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n pprotocol3 pprotocol3.vmt
echo -e "1\n 1\n 1\n 2\n 1\n 1\n 2\n 1\n 2\n 2\n 1\n 1\n 1\n 1\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n psystem psystem.vmt

echo "converting inductive invariant"
python2 ../converter.py output/pprotocol1/pprotocol1.inv > pprotocol1.inv
python2 ../converter.py output/pprotocol2/pprotocol2.inv > pprotocol2.inv
python2 ../converter.py output/pprotocol3/pprotocol3.inv > pprotocol3.inv
python2 ../converter.py output/psystem/psystem.inv > psystem.inv
