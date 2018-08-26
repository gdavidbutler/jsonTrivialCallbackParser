CFLAGS = -Wall -Wextra -Iinc -O3 -g

all: main print

clean:
	rm -f json.o

clobber: clean
	rm -f main print

json.o: src/json.c inc/json.h
	cc $(CFLAGS) -c -o json.o src/json.c

main: test/main.c inc/json.h json.o
	cc $(CFLAGS) -o main test/main.c json.o

print: test/print.c inc/json.h json.o
	cc $(CFLAGS) -o print test/print.c json.o

run: main print
	./main 0
	./main 0 test/test.json
	./main 1 test/test.json
	./print < test/test.json
