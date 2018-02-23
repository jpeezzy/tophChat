#ifndef SERVER_H
#define SERVER_H

unsigned int numOfUsers = 0;
//setups the server 
int setupServer(int portNum);
//returns the list of users in the server
int getUsers();
int closeSocket(int *newsockfd, int *sockfd);
int mainLoop(int argc, char *argv[]);

#endif 
