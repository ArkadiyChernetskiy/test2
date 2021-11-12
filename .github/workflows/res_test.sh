#! /bin/bash

./.github/workflows/generate_arrays.py > test.txt
A=$(head -n 1 test.txt)
echo $A
tail -n +2 test.txt > test_without_sizes.txt
./.github/workflows/generate_answer.py > answer.txt
./a.out $A < test_without_sizes.txt > out.txt
./.github/workflows/check_answer.py