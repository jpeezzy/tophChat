CC := gcc 
#POSIX_C_SOURCE allows c99 to use getaddrinfo function 
CFLAGS := -std=c99 -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L
LFLAGS := -Wall -lm
DEBUG := -DDEBUG
MAIN := -DMAIN
testServerBin=testServer
executable_file := server_main client_main $(testServerBin)

# compiler flags for GTK usage
GTKINC := `pkg-config --cflags gtk+-2.0`
GTKLIBS := `pkg-config --libs gtk+-2.0`

all: fifo_debug.o server_main.o client_main.o utils.o protocol.o tcpPacket_debug.o tcpPacket.o tcpGUI_debug.o tcpGUI.o server_back_end_debug.o server_back_end.o tophChatUsers.o GTK.o rsa.o GTKMain.o emoji.o serverGUI.o encrypt.o rsa_DEBUG.o serverGUI_DEBUG.o test_emoji.o 

#####################END OF TDD AREA################################

#Executables

server_main: tcpPacket_debug.o server_back_end_debug.o server_main.o utils.o protocol.o tophChatUsers.o constants.h fifo.o
	$(CC)  $(^) -o $(@)	$(LFLAGS)

client_main: tcpPacket_debug.o protocol.o tcpGUI_debug.o client_main.o utils.o
	$(CC)  $(^) -o $(@)	$(LFLAGS)

ChatGUI: GTKMain.o GTK.o emoji.o protocol.o utils.o tcpPacket_debug.o tcpGUI_debug.o fifo_debug.o 
	$(CC) $(CFLAGS) $(GTKLIBS) $(^) -o $(@)

fifo_debug.o: fifo.c fifo.h constants.h
	$(CC) -c fifo.c $(CFLAGS) $(DEBUG) -o $(@)
	
fifo.o: fifo.c fifo.h constants.h
	$(CC) -c fifo.c $(CFLAGS) -o $(@)
	
serverGUI: serverGUI.o 
	$(CC) $(LFLAGS) $(GTKLIBS) $(^) -o $(@)	

#Object files
server_main.o: server_main.c
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

client_main.o: client_main.c 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

utils_debug.o: utils.c utils.h constants.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

utils.o: utils.c utils.h constants.h
	$(CC) -c $(<) $(CFLAGS) -o $(@)

protocol.o: protocol.c protocol.h protocol_const.h utils.h constants.h server_back_end.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tcpPacket_debug.o: tcpPacket.c tcpPacket.h fifo.h tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tcpPacket.o: tcpPacket.c tcpPacket.h fifo.h tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) -o $(@)

tcpGUI_debug.o: tcpGUI.c tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(MAIN) $(DEBUG) -o $(@)

tcpGUI.o: tcpGUI.c tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) -o $(@)

server_back_end_debug.o: server_back_end.c server_back_end.h constants.h
	$(CC) -c $(<) $(CFLAGS) $(MAIN) $(DEBUG) -o $(@)

server_back_end.o: server_back_end.c server_back_end.h constants.h
	$(CC) -c $(<) $(CFLAGS) -o $(@)



tophChatUsers.o: ./users/tophChatUsers.c ./users/tophChatUsers.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

GTK.o:  GTK.c GTK.h emoji.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

rsa.o: rsa.c rsa.h 
	$(CC) $(CLFAGS) -c $(<) -o $(@)

GTKMain.o: GTKMain.c GTK.h emoji.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

emoji.o: emoji.c emoji.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

serverGUI.o: serverGUI.c
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

encrypt.o: encrypt.c encrypt.h
	$(CC) $(CFLAGS) -c $(<) -o $(@)

#Test object files

rsa_DEBUG.o: rsa.c rsa.h
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) -c $(<) -o $(@)

serverGUI_DEBUG.o: serverGUI.c
	$(CC) $(CFLAGS) $(DEBUG) $(GTKINC) -c $(<) -o $(@)	

test_emoji.o: testEmoji.c emoji.h
	$(CC) $(CFLAGS) $(DEBUG) $(GTKINC) -c $(<) -o $(@)

#Test Executables

test_emoji: test_emoji.o emoji.o
	$(CC) $(LFLAGS) $(DEBUG) $(GTKLIBS) $(^) -o $(@)

test_RSA: rsa_DEBUG.o 
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) $(^) -o $(@)

test_serverGUI: serverGUI_DEBUG.o
	$(CC) $(LFLAGS) $(DEBUG) $(GTKLIBS) $(^) -o $(@)

test_Encrypt: encrypt_DEBUG.o
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) $(^) -o $(@)

test_server_back_end: tcpPacket_debug.o server_back_end_debug.o utils_debug.o protocol.o tophChatUsers.o constants.h fifo_debug.o 
	$(CC)  $(^) $(DEBUG) $(MAIN) -o $(@)	$(LFLAGS)

test_server_main: tcpPacket_debug.o server_back_end.o server_main.o utils_debug.o protocol.o tophChatUsers.o constants.h fifo_debug.o 
	$(CC)  $(^) $(DEBUG) $(MAIN) -o $(@)	$(LFLAGS)

test_tcp_GUI: tcpPacket_debug.o protocol.o tcpGUI_debug.o client_main.o utils_debug.o fifo_debug.o
	$(CC)  $(^) $(DEBUG) $(MAIN) -o $(@)	$(LFLAGS)

test_client_main: tcpPacket_debug.o protocol.o tcpGUI.o client_main.o utils_debug.o fifo_debug.o
	$(CC)  $(^) $(DEBUG) $(MAIN) -o $(@)	$(LFLAGS)

clean:
	rm -rf *.o $(executable_file) -v serverGUI ChatGUI serverGUI test_RSA test_serverGUI test_emoji
