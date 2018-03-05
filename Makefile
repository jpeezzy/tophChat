CC := gcc 
CXX := g++  
LKP := -lgmock -lgtest -lpthread -lm
#POSIX_C_SOURCE allows c99 to use getaddrinfo function 
CFLAGS := -std=c99 -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L
LFLAGS := -Wall
CPFLAGS := -Wall -std=c++0x -g
DEBUG := -DDEBUG
MAIN := -DMAIN
testServerBin=testServer
executable_file := tdd_protocol tdd_utils tdd_fifo tdd_client_server tdd_simple_IO tdd_tcpPacket tdd_tcpGUI tdd_server_back_end $(testServerBin)

# compiler flags for GTK usage
GTKINC := `pkg-config --cflags gtk+-2.0`
GTKLIBS := `pkg-config --libs gtk+-2.0`

################################TDD AREA##########################
#TEST FIFO
tdd_fifo: fifo_debug.o fifo_test.o
	$(CXX) $(^) -o $(@) $(LKP)
	#./$(@)
fifo_test.o: fifoTest.cc fifo.c 
	$(CXX) -c fifoTest.cc $(CPFLAGS) -o $(@)
fifo_debug.o: fifo.c fifo.h constants.h
	$(CC) -c fifo.c $(CFLAGS) $(DEBUG) -o $(@)

#NETWORK AND CLIENT TEST CLASS
# networkTestClass.o: networkTestClass.cc networkTestClass.h constants.h tcpGUI.h server_back_end.h 
# 	$(CXX) -c $(<) $(CPFLAGS) -o $(@)

testServer: server_back_end.c server_back_end.h testString.h
	$(CC) $(<) $(CFLAGS) $(DEBUG) -o $(@)

#JOINT TEST OF CLIENT AND SERVER
tdd_client_server: client_main_test.o client_main_debug.o tcpGUI_debug.o tcpPacket_debug.o fifo_debug.o 
	$(CXX) $(^) -o $(@) $(LKP)
	#./$(@)
client_main_test.o: client_main_test.cc client_main.c 
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
client_main_debug.o: client_main.c constants.h tcpGUI.h tcpPacket.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)
server_main_test.o: server_main_test.cc server_main.c 
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
server_main_debug.o: server_main.c constants.h tcpGUI.h tcpPacket.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)


#TEST SIMPLE I/O FUNCTION LIKE SEND, FETCH 
tdd_tcpGUI: tcpGUI_test.o tcpGUI_debug.o 
	$(CXX) $(^) -o $(@) $(LKP)
tcpGUI_test.o: tcpGUI_test.cc tcpGUI.c
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
tcpGUI_debug.o: tcpGUI.c tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tdd_tcpPacket: tcpPacket_test.o tcpPacket_debug.o testServer
	$(CXX) tcpPacket_test.o tcpPacket_debug.o -o $(@) $(LKP)
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

#TEST UTILITIES
tdd_utils: utils_test.o utils.o
	$(CXX) $(^) -o $(@) $(LKP)
utils_test.o: utils_test.cc utils.h 
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
utils.o: utils.c utils.h constants.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

#TEST PROTOCOLS DECODER
tdd_protocol: protocol_test.o protocol.o utils.o
	$(CXX) $(^) -o $(@) $(LKP)
protocol_test.o: protocol_test.cc protocol.h protocol_const.h utils.h constants.h server_back_end.h
	$(CXX) -c $(<) $(CPFLAGS) -o $(@)
protocol.o: protocol.c protocol.h protocol_const.h utils.h constants.h server_back_end.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)
#####################END OF TDD AREA################################

#Executables

ChatGUI: GTKMain.c GTK.o
	$(CC) $(CFLAGS) $(GTKLIBS) $(^) -o $(@)

serverGUI: serverGUI.o
	$(CC) $(LFLAGS) $(GTKLIBS) $(^) -o $(@)	

#Object files

GTK.o:  GTK.c GTK.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

rsa.o: rsa.c rsa.h 
	$(CC) $(CLFAGS) -c $(<) -o $(@)

serverGUI.o: serverGUI.c
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

#Test object files

rsa_DEBUG.o: rsa.c rsa.h
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) -c $(<) -o $(@)

serverGUI_DEBUG.o: serverGUI.c
	$(CC) $(CFLAGS) $(DEBUG) $(GTKINC) -c $(<) -o $(@)	

#Test Executables

test_RSA: rsa_DEBUG.o 
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) $(^) -o $(@)

test_serverGUI: serverGUI_DEBUG.o
	$(CC) $(LFLAGS) $(DEBUG) $(GTKLIBS) $(^) -o $(@)

#clean 

clean:
	rm -rf *.o $(executable_file) -v serverGUI ChatGUI serverGUI test_RSA test_serverGUI
