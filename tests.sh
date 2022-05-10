export PYTHONPATH=$(pwd)/ic3po/pysmt:$PYTHONPATH


echo 'leader election'
pushd .
cd leader
bash run.sh
popd
echo -e 'leader election done!\n'

echo 'distributed'
pushd .
cd toy_lock
bash run.sh
popd
echo -e 'distributed lock done!\n'

echo 'two phase commit'
pushd .
cd 2pc
bash run.sh
popd
echo -e 'two phase commit done!\n'

echo 'sharded hash table'
pushd .
cd sht
bash run.sh
popd
echo -e 'sharded hash table done!\n'

echo 'raft'
pushd .
cd ruft
bash run.sh
popd
echo -e 'raft done!\n'

echo 'multi-paxos'
pushd .
cd multipaxos
bash run.sh
popd
echo -e 'multi-paxos done!\n'
