CFLAGS=-W -Wall -pedantic -g
LDFLAGS=-g
OBJECTS=testharness.o packetdriver.o
LIBRARIES=./libTH.a

mydemo: $(OBJECTS)
	gcc -o $@ $(LDFLAGS) $^ $(LIBRARIES) -lpthread

clean:
	rm -f packetdriver.o mydemo

packetdriver.o: packetdriver.c
