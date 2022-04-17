CFLAGS = -I. -Os -g

all: json.o coders main dom print

clean:
	rm -f json.o coders main dom print

json.o: json.c json.h
	cc $(CFLAGS) -c json.c

coders: test/coders.c json.h json.o
	cc $(CFLAGS) -o coders test/coders.c json.o

main: test/main.c json.h json.o
	cc $(CFLAGS) -o main test/main.c json.o

dom: test/dom.c json.h json.o
	cc $(CFLAGS) -o dom test/dom.c json.o

print: test/print.c json.h json.o
	cc $(CFLAGS) -o print test/print.c json.o

check: coders main dom print
	./coders
	./main 0 test/test.json
	./main 1 test/test.json
	./dom test/test.json
	./print < test/test.json
