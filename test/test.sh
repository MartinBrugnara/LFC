#!/bin/bash

if [[ -z $1 ]];then
    echo "usage: ./test.sh <test_dir>"
    exit 1
fi

cd .. && make clean && make && cd exam

for F in $1/*; do
    echo "------------------------------------------------------------------------"
    echo "Testing $F"
    ../interpreter < $F
    echo
done;
