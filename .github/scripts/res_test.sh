#! /bin/bash

script_path=./.github/scripts

python $script_path/generate_arrays.py > test.txt
A=$(head -n 1 test.txt)
tail -n +2 test.txt > test_without_sizes.txt
python $script_path/generate_answer.py > answer.txt
./a.out $A < test_without_sizes.txt > out.txt
python $script_path/check_answer.py
