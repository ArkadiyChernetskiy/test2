all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./.github/scripts/res_test.sh

memory_test:
	./.github/scripts/memory_tests.sh;

value_test:
	./.github/scripts/value_tests.sh;

clean:
	rm a.out
	rm out.txt
	rm *.txt
