CC = gcc
CFLAGS = -std=c99 -Wall -g 
LFLAGS = -Wall 
DEBUG := -DDEBUG -g
MAIN := -DMAIN

# compiler flags for GTK usage

GTKLIBS := `pkg-config --libs gtk+-2.0`

#tar

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

TestServer:

TestClient:

#clean

clean: 
	rm -rf *./bin/*.o $(test_executables)
