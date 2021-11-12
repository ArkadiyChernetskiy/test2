all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./a.out > out.txt
	echo "Hello world" > answer.txt 
	diff out.txt answer.txt
clean:
	rm a.out
	rm out.txt
