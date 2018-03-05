CC = gcc
CFLAGS = -std=c99 -Wall -g 
LFLAGS = -Wall -lm
DEBUG := -DDEBUG -g
MAIN := -DMAIN

# compiler flags for GTK usage

GTKLIBS := `pkg-config --libs gtk+-2.0`

#tar
all: TestClientGUI TestServerGUI TestRSA TestServer TestClient ServerGUI Server Client

tar: 
	gtar cvzf Chat_Alpha_src.tar.gz bin doc src COPYRIGHT INSTALL README Makefile

#test executables
test_executables := ./bin/TestClientGUI ./bin/TestServerGUI ./bin/TestRSA

TestClientGUI: ./src/GTK.o ./src/GTKMain.c
	$(CC) $(CFLAGS) ./src/GTK.o ./src/GTKMain.c -o ./bin/TestClientGUI

TestServerGUI: ./src/serverGUI_DEBUG.o
	$(CC) $(LFLAGS) $(DEBUG) $(GTKLIBS) $(^) -o ./bin/TestServerGUI

TestRSA: ./src/rsa_DEBUG.o
	$(CC) $(LFLAGS) $(DEBUG) $(MAIN) $(^) -o ./bin/TestRSA

TestServer: ./src/tcpPacket_debug.o ./src/server_back_end_debug.o ./src/server_main.o
	$(CC)  $(DEBUG) $(MAIN) $(^) -o ./bin/TestServer $(LFLAGS)

TestClient: ./src/tcpPacket_debug.o ./src/protocol.o ./src/tcpGUI_debug.o ./src/client_main.o ./src/utils.o
	$(CC)  $(DEBUG) $(MAIN) $(^) -o ./bin/TestClient $(LFLAGS)

ServerGUI: ./src/serverGUI.o
	$(CC) $(LFLAGS) $(GTKLIBS) $(^) -o ./bin/ServerGUI

Server: ./src/tcpPacket.o ./src/server_back_end.o ./src/server_main.o
	$(CC)  $(MAIN) $(^) -o ./bin/Server $(LFLAGS)

Client: ./src/tcpPacket_debug.o ./src/protocol.o ./src/tcpGUI_debug.o ./src/client_main.o ./src/utils.o
	$(CC)  $(MAIN) $(^) -o ./bin/Client $(LFLAGS)
#clean

clean: 
	rm -rf *./bin/*.o $(test_executables)
