all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./.github/scripts/res_test.sh

memory_test:
	./.github/scripts/memory_tests.sh;

value_test:
	n=1; \
    while [ $${n} -lt 101 ] ; do \
        printf "test number $$n\n" ;\
		nn=$$(./.github/scripts/value_test.sh);\
		printf "\n$$nn\n";\
        n=`expr $$n + 1`; \
    done; \

clean:
	rm a.out
	rm out.txt
	rm *.txt
