echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system leader_system.ivy leader.vmt

echo "running ic3po"

echo -e "3\n 2\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n leader leader.vmt

echo "converting inductive invariant"

python2 ../converter.py output/leader/leader.inv > leader.inv
