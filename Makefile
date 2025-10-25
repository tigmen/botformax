all: index

index: client.o trace.o net.o
	gcc trace.o net.o client.o -lcurl -ljson-c -o index

client.o: client.c
	gcc -c -g client.c

trace.o: trace/trace.c trace/trace.h
	gcc -c trace/trace.c

net.o: net.h net.c
	gcc -c net.c

clean:
	rm -rf *.o vgcore.* log index

