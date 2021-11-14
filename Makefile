all: build

build:
	g++ -std=c++17 hello.cpp

test1:
	./.github/scripts/res_test.sh

memory_test:
	n=1; \
    while [ $${n} -lt 11 ] ; do \
        printf "test number $$n\r" ;\
		./.github/scripts/mem_test.sh;\
        n=`expr $$n + 1`; \
    done; \

value_test:
	n=1; \
    while [ $${n} -lt 101 ] ; do \
        printf "test number $$n\r" ;\
		./.github/scripts/mem_test.sh;\
        n=`expr $$n + 1`; \
    done; \

clean:
	rm a.out
	rm out.txt
	rm *.txt
