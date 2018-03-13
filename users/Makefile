CC := gcc 
#POSIX_C_SOURCE allows c99 to use getaddrinfo function 
CFLAGS := -std=c99 -Wall -Wextra -g -D_POSIX_C_SOURCE=200112L
LFLAGS := -Wall -lm
INCLUDE = users/

tophChatUsers.o: tophChatUsers.c tophChatUsers.h
	$(CC) -i$(INCLUDE) $(CFLAGS) $(GTKINC) $(<) -o $(@)

DEBUG: tophChatUsers.c tophChatUsers.h
	$(CC) $(CFLAGS) -DDEBUG $(GTKINC) $(<) -o $(@)

clean: 
	rm tophChatUsers.o DEBUG
