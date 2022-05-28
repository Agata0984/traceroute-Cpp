CC = g++
CFLAGS = -Wall -Wextra -std=c++11   -lm
OBJECTS = traceroute.o functions.o
all: $(OBJECTS) 
	$(CC) $(CFLAGS) $^ -o traceroute

clean:
	rm traceroute.o || true
	
	rm functions.o || true
distclean: clean
	rm traceroute || true
