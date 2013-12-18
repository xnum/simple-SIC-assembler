CC		= g++
CFLAGS	= -lm -Wall

all: main

clean:
	rm *.o main

main: input.o main.o
	$(CC) $(CFLAGS) -o main main.o input.o
input.o: input.cpp input.h
	$(CC) $(CFLAGS) -c -o input.o input.cpp
main.o: main.cpp 
	$(CC) $(CFLAGS) -c -o main.o main.cpp
