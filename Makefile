CC = gcc -c
CC+= g++ -c
LK+= g++ -lgtest -lpthread
CFLAGS = -std=c99 -Wall 
CFLAGS+= -Wall 
DEBUG= -DDEBUG
executable_file= fifo_debug.o

tdd_fifo: fifo_debug.o fifo_test.o
	$(LK+) fifo_debug.o fifo_test.o -o $(@) 
fifo_test.o: fifoTest.cpp fifo.c 
	$(CC+) fifoTest.cpp -o fifo_test.o 
fifo_debug.o: fifo.c fifo.h constants.h
	$(CC) fifo.c $(CFLAGS) $(DEBUG) -o $(@)

clean:
	rm -rf *.o $(executable_file)
