all: a.out

a.out:
	g++ -std=c++17 hello.cpp

test: a.out
	./a.out

clean:
	rm a.out
