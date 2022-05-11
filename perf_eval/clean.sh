#!/bin/bash 

echo "Cleaning Toy Lock"
cd ./toy_lock
rm -f toy_lock_system.cpp
rm -f toy_lock_system.h
rm -f toy_lock_server
rm -f *~
cd ..

echo "Cleaning Leader Election"
cd ./leader
rm -f leader_system.cpp
rm -f leader_system.h
rm -f leader_server
rm -f *~
cd ..

echo "Cleaning 2PC"
cd ./2pc
rm -f twophase_system.cpp
rm -f twophase_system.h
rm -f twophase_server
rm -f *~
cd ..

echo "Cleaning SHT"
cd ./sht
rm -f sht_system.cpp
rm -f sht_system.h
rm -f sht_client 
rm -f sht_server
rm -f *.csv
rm -f *~
cd ..

echo "Cleaning MultiPaxos"
cd ./multipaxos
rm -f paxos_system.cpp
rm -f paxos_system.h
rm -f paxos_server
rm -f paxos_client
rm -f *.csv
rm -f *~
cd ..

echo "Cleaning Raft"
cd ./raft
rm -f raft_system.cpp
rm -f raft_system.h
rm -f raft_client
rm -f raft_server
rm -f *.csv
rm -f *~
cd ..

rm -f *~

echo "Done!"
