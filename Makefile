all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./a.out
clean:
	rm a.out
