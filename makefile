CC=g++ -Wall -g

all: main

main: helper.o main.o circularQueue.o helper.o
	$(CC) -pthread main.o circularQueue.o helper.o -o main 

main.o: helper.cc main.cc
	$(CC) -c main.cc

circularQueue.o: circularQueue.cc circularQueue.h
	$(CC) -c circularQueue.cc

helper.o: helper.cc helper.h
	$(CC) -c helper.cc

tidy:
	rm -f *.o core

clean:
	rm -f main producer consumer *.o core
