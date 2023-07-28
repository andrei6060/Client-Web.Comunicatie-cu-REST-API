CC=g++
CFLAGS=-I.

client: clien.cpp requests.cpp helpers.cpp buffer.cpp
	$(CC) -w -o clien clien.cpp requests.cpp helpers.cpp buffer.cpp -Wall

run: client
	./clien

clean:
	rm -f *.o clien
