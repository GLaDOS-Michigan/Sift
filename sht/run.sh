echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system sht_system.ivy sht.vmt

echo "running ic3po"

echo -e "1\n 2\n 2\n 3\n 4\n 2\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n sht sht.vmt

echo "converting inductive invariant"

python2 ../converter.py output/sht/sht.inv > sht.inv
