#! /bin/bash

python ./tests/generate_arrays.py > test.txt
A=$(head -n 1 test.txt)
echo $A
tail -n +2 test.txt > test_without_sizes.txt
python ./tests/generate_answer.py > answer.txt
./a.out $A < test_without_sizes.txt > out.txt
python ./tests/check_answer.py