#!/bin/bash


# Compile RAFT, MULTIPAXOS, SHT, TOYLOCK, 2PC, and Leader Election to cpp
#---------

#TOYLOCK
echo
echo $"compiling toy_lock ivy to cpp"
cd ../toy_lock
ivy_to_cpp target=repl isolate=iso_impl toy_lock_system.ivy
sed -i '32 i #include "config.h"' toy_lock_system.cpp
sed -i '33 i #include<unistd.h>' toy_lock_system.cpp
mv toy_lock_system.cpp ../perf_eval/toy_lock
mv toy_lock_system.h ../perf_eval/toy_lock
cd ../perf_eval/toy_lock
echo "compiling toy_lock system"
g++ -w -std=c++11 -o toy_lock_server toy_lock_system.cpp -pthread
cd ..



#Leader
echo
echo $"compiling leader ivy to cpp"
cd ../leader
ivy_to_cpp target=repl isolate=iso_impl leader_system.ivy
sed -i '32 i #include "config.h"' leader_system.cpp
sed -i '33 i #include<unistd.h>' leader_system.cpp
mv leader_system.cpp ../perf_eval/leader
mv leader_system.h ../perf_eval/leader
cd ../perf_eval/leader
echo "compiling leader system"
g++ -w -std=c++11 -o leader_server leader_system.cpp -pthread
cd ..

#2pc
echo
echo $"compiling 2pc ivy to cpp"
cd ../2pc
ivy_to_cpp target=repl isolate=iso_impl twophase_system.ivy
sed -i '32 i #include "config.h"' twophase_system.cpp
sed -i '33 i #include<unistd.h>' twophase_system.cpp
mv twophase_system.cpp ../perf_eval/2pc
mv twophase_system.h ../perf_eval/2pc
cd ../perf_eval/2pc
echo "compiling 2pc system"
g++ -w -std=c++11 -o twophase_server twophase_system.cpp -pthread
cd ..

#SHT
echo
echo $"compiling sht ivy to cpp"
cd ../sht
ivy_to_cpp target=repl isolate=impl sht_system.ivy
patch < ../perf_eval/sht/sht_h.patch >/dev/null
patch < ../perf_eval/sht/sht_cpp.patch >/dev/null
mv sht_system.cpp ../perf_eval/sht
mv sht_system.h ../perf_eval/sht
cd ../perf_eval/sht
echo "compiling sht system" 
g++ -w -std=c++11 -o sht_server sht_system.cpp -pthread
echo "compiling sht client"
g++ -w -std=c++11 -o sht_client sht_client.cpp -pthread
cd ..

#MULTIPAXOS
echo
echo $"compiling multipaxos ivy to cpp"
cd ../multipaxos
ivy_to_cpp target=repl isolate=iso_impl paxos_system.ivy
patch < ../perf_eval/multipaxos/paxos_h.patch >/dev/null
patch < ../perf_eval/multipaxos/paxos_cpp.patch >/dev/null
mv paxos_system.cpp ../perf_eval/multipaxos
mv paxos_system.h ../perf_eval/multipaxos
cd ../perf_eval/multipaxos
echo "compiling paxos system"
g++ -w -std=c++11 -o paxos_server paxos_system.cpp -pthread
echo "compiling paxos client"
g++ -w -std=c++11 -o paxos_client paxos_client.cpp -pthread
cd ..

#RAFT
echo
echo $"compiling raft ivy to cpp"
cd ../raft
ivy_to_cpp target=repl isolate=iso_impl raft_system.ivy
patch < ../perf_eval/raft/raft_h.patch >/dev/null
patch < ../perf_eval/raft/raft_cpp.patch >/dev/null
mv raft_system.cpp ../perf_eval/raft
mv raft_system.h ../perf_eval/raft
cd ../perf_eval/raft
echo "compiling raft system"
g++ -w -std=c++11 -o raft_server raft_system.cpp -pthread
echo "compiling raft client"
g++ -w -std=c++11 -o raft_client raft_client.cpp -pthread

RETURN="$?"
if [ "${RETURN}" != "0" ]; then
    echo "Building dependencies failed!"
    exit 1
fi

echo
echo $"Done! Success!" 
