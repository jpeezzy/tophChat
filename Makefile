CXX = gcc 
FLAGS = -std=c99
server.o: server.c server.h
	$(CXX) $(FLAGS) server.c -o $@

client.o: client.c
	$(CXX) $(FLAGS) client.c -o $(@)
