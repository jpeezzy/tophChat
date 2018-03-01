CC := gcc 
CCP := g++  
LKP := -lgmock -lgtest -lpthread 
CFLAGS := -std=c99 -Wall -Wextra -g
CPFLAGS := -Wall -std=c++0x -g
DEBUG := -DDEBUG
executable_file := tdd_fifo

tdd_fifo: fifo_debug.o fifo_test.o
	$(CCP) fifo_debug.o fifo_test.o -o $(@) $(LKP)
	#./$(@)
fifo_test.o: fifoTest.cpp fifo.c 
	$(CCP) -c fifoTest.cpp $(CPFLAGS) -o $(@)
fifo_debug.o: fifo.c fifo.h constants.h
	$(CC) -c fifo.c $(CFLAGS) $(DEBUG) -o $(@)

clean:
	rm -rf *.o $(executable_file)
