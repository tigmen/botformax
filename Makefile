all: index

index: testclient.o trace.o
	gcc trace.o testclient.o -lssl -ljson-c -o index

testclient.o: testclient.c
	gcc -c testclient.c

trace.o: trace/trace.c trace/trace.h
	gcc -c trace/trace.c

clean:
	rm -rf *.o vgcore.*

