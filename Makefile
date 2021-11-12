all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./.github/res_test.sh
clean:
	rm a.out
	rm out.txt
