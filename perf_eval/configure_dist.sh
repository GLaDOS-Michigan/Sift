\#!/bin/bash 


for ips in "$@"
do
    echo "copying ./perf_eval directory to the following location : $ips"
    cd ..
    scp -r perf_eval $ips
    cd perf_eval
done


echo "DONE!" 
