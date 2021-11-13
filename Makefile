all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./.github/scripts/res_test.sh

memory_test:
	./.github/scripts/mem_test.sh

value_test:
	./.github/scripts/value_test.sh

clean:
	rm a.out
	rm out.txt
	rm *.txt
