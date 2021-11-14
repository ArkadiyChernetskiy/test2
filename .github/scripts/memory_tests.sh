#! /bin/bash

script_path=./.github/scripts

for ((I=1; I<11; I=I+1))
do
    echo "Test $I"
    $script_path/mem_test.sh
done