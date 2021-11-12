all: build

build:
	g++ -std=c++17 hello.cpp

test1: build
	./a.out

clean:
	rm a.out
