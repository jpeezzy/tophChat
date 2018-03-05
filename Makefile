CC := gcc 
#POSIX_C_SOURCE allows c99 to use getaddrinfo function 
CFLAGS := -std=c99 -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L
LFLAGS := -Wall -lm
DEBUG := -DDEBUG
MAIN := -DMAIN
testServerBin=testServer
executable_file := $(testServerBin)

# compiler flags for GTK usage
GTKINC := `pkg-config --cflags gtk+-2.0`
GTKLIBS := `pkg-config --libs gtk+-2.0`


#####################END OF TDD AREA################################

#Executables

server_main: tcpPacket_debug.o server_back_end_debug.o server_main.c
	$(CC)  $(^) -o $(@)	$(LFLAGS)
client_main: tcpPacket_debug.o protocol.o tcpGUI_debug.o client_main.c utils.o
	$(CC)  $(^) -o $(@)	$(LFLAGS)

ChatGUI: GTKMain.c GTK.o
	$(CC) $(CFLAGS) $(GTKLIBS) $(^) -o $(@)

serverGUI: serverGUI.o 
	$(CC) $(LFLAGS) $(GTKLIBS) $(^) -o $(@)	

#Object files

utils.o: utils.c utils.h constants.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

protocol.o: protocol.c protocol.h protocol_const.h utils.h constants.h server_back_end.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tcpPacket_debug.o: tcpPacket.c tcpPacket.h fifo.h tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

tcpGUI_debug.o: tcpGUI.c tcpGUI.h constants.h 
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)

server_back_end_debug.o: server_back_end.c server_back_end.h constants.h
	$(CC) -c $(<) $(CFLAGS) $(DEBUG) -o $(@)



tophChatUsers.o: tophChatUsers.c tophChatUsers.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

GTK.o:  GTK.c GTK.h
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

rsa.o: rsa.c rsa.h 
	$(CC) $(CLFAGS) -c $(<) -o $(@)

serverGUI.o: serverGUI.c
	$(CC) $(CFLAGS) $(GTKINC) -c $(<) -o $(@)

encrypt.o: encrypt.c encrypt.h
	$(CC) $(CFLAGS) -c $(<) -o $(@)

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
