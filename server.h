#ifndef SERVER_H
#define SERVER_H
#include "tcpGUI.h"
#include "constants.h"

#include <socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

typedef struct chatterServer userServer;

// user as seen from the server perspective
struct chatterServer
{
    int socket;        // socket that the user is associated for this session
    char userName[50]; // used as the folder name for storing in the database
};

unsigned int numOfUsers = 0;
//setups the server
int setupServer(int portNum);

int closeServer(struct allRoom *roomList);

//returns the list of users in the server
int getUsers();
int closeSocket(int *newsockfd, int *sockfd);
int mainLoop(int argc, char *argv[]);

// general message handler, return the type of error based on the input message
// and handle it if it's a special request, otherwise pass to the room if
// it's a regular message
int parseRequest(char message[500], struct chatterServer user);

// update user profile, verify their identity
// return a pointer to the user information for this session
struct chatterServer *parseNewConnection(int socket, char message[500]);

// add the new connection to the watchlist
addConnection(fd_set *conList, int socket);

#endif
