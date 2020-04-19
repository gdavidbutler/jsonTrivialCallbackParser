CFLAGS = -I. -Os -g

all: json.o main print

clean:
	rm -f json.o main print

json.o: json.c json.h
	cc $(CFLAGS) -c json.c

main: test/main.c json.h json.o
	cc $(CFLAGS) -o main test/main.c json.o

print: test/print.c json.h json.o
	cc $(CFLAGS) -o print test/print.c json.o

check: main print
	./main 0
	./main 0 test/test.json
	./main 1 test/test.json
	./print < test/test.json
