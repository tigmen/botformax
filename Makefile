all: index

index: _client.o _trace.o _net.o
	gcc _trace.o _net.o _client.o -lcurl -ljson-c -o index

_client.o: client.c
	gcc -c -g client.c -o _client.o

_trace.o: trace/trace.c trace/trace.h
	gcc -c trace/trace.c -o _trace.o

_net.o: net.h net.c
	gcc -c net.c -o _net.o

clean:
	rm -rf *.o vgcore.* log index

