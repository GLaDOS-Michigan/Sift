echo "generating vmt files"

python2 ../ic3po/ivy/ivy/ivy_to_vmt.py isolate=system toy_lock_system.ivy toy_lock.vmt

echo "running ic3po"

echo -e "3\n 2\n" | python2 ../ic3po/ic3po.py -g univ -v 2 -n toy_lock toy_lock.vmt

echo "converting inductive invariant"

python2 ../converter.py output/toy_lock/toy_lock.inv > toy_lock.inv
