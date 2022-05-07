echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system twophase_system.ivy 2pc.vmt

echo "running ic3po"

echo -e "2\n 1\n 1\n 5\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n 2pc 2pc.vmt

echo "converting inductive invariant"

python2 ../converter.py output/2pc/2pc.inv > 2pc.inv
