CC := gcc 
CXX := g++  
LKP := -lgmock -lgtest -lpthread 
#POSIX_C_SOURCE allows c99 to use getaddrinfo function 
CFLAGS := -std=c99 -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L
CPFLAGS := -Wall -std=c++0x -g
DEBUG := -DDEBUG
executable_file := tdd_fifo tdd_client_server tdd_simple_IO tdd_tcpPacket tdd_tcpGUI tdd_server_back_end

################################TDD AREA##########################
#TEST FIFO
tdd_fifo: fifo_debug.o fifo_test.o
	$(CXX) $(^) -o $(@) $(LKP)
	#./$(@)
fifo_test.o: fifoTest.cc fifo.c 
	$(CXX) -c fifoTest.cc $(CPFLAGS) -o $(@)
fifo_debug.o: fifo.c fifo.h constants.h
	$(CC) -c fifo.c $(CFLAGS) $(DEBUG) -o $(@)

#JOINT TEST OF CLIENT AND SERVER
tdd_client_server: client_main_test.o client_main_debug.o tcpGUI_debug.o tcpPacket_debug.o fifo_debug.o 
	$(CXX) $(^) -o $(@) $(LKP)
	#./$(@)
client_main_test.o: client_main_test.cc client_main.c 
	$(CXX) -c client_main_test.cc $(CPFLAGS) -o $(@)
client_main_debug.o: client_main.c constants.h tcpGUI.h tcpPacket.h 
	$(CC) -c client_main.c $(CFLAGS) $(DEBUG) -o $(@) 
server_main_test.o: server_main_test.cc server_main.c 
	$(CXX) -c server_main_test.cc $(CPFLAGS) -o $(@)
server_main_debug.o: server_main.c constants.h tcpGUI.h tcpPacket.h 
	$(CC) -c server_main.c $(CFLAGS) $(DEBUG) -o $(@) 


#TEST SIMPLE I/O FUNCTION LIKE SEND, FETCH 
tdd_tcpGUI: tcpGUI_test.o tcpGUI_debug.o 
	$(CXX) $(^) -o $(@) $(LKP)
tcpGUI_test.o: tcpGUI_test.cc tcpGUI.c
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
tcpGUI_debug.o: tcpGUI.c tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tdd_tcpPacket: tcpPacket_test.o tcpPacket_debug.o
	$(CXX) $(^) -o $(@) $(LKP)
tcpPacket_test.o: tcpPacket_test.cc tcpPacket.c 
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
tcpPacket_debug.o: tcpPacket.c tcpPacket.h fifo.h tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tdd_server_back_end: server_back_end_test.o server_back_end_debug.o
	$(CXX) $(^) -o $(@) $(LKP)
server_back_end_test.o: server_back_end_test.cc server_back_end.c 
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
server_back_end_debug.o: server_back_end.c server_back_end.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

#####################END OF TDD AREA################################

clean:
	rm -rf *.o $(executable_file)
