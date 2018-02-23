/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <strings.h> //bzero
#include <sys/select.h> //io multiplexing 

#include "server.h" //holds numOfUsers

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	mainLoop(argc, argv);
	return 0; 
}

int getUsers()
{
	if(numOfUsers == 0)
	{
		printf("You have 0 users!\n");
		return 0;
	}
	return numOfUsers;
}

int closeSocket(int *newsockfd, int *sockfd)
{
	assert(newsockfd);
	assert(sockfd);
	close(*newsockfd);
	close(*sockfd);
	printf("Server Closed Sockets Successfully!\n");
	return 0;
}

int mainLoop(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	fd_set active_fd_set;
	struct timeval tv; 
	socklen_t clilen;
	char buffer[256];
	int maxSocket = 0;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	printf("Usage: listUser UserIds\n");

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	//check constantly to see if there's a new client;
	//first zero out the set
	FD_ZERO (&active_fd_set);
	FD_SET (sockfd, &active_fd_set);
	listen(sockfd,5);
	fd_set childrenSet;
	FD_ZERO (&childrenSet);
	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, 
				(struct sockaddr *) &cli_addr, 
				&clilen);
		FD_SET(newsockfd,  &childrenSet);
		//after client connects with accept() method to server,
		/*Initialize the set of active Sockets */

		printf("client connected!\n");
		maxSocket = newsockfd + 1;
		numOfUsers++;
		printf("There are currently %d Users\n", numOfUsers);
		printf("there are %d file descriptors \n", 
				select(maxSocket, &childrenSet, NULL, NULL, NULL));
		if (newsockfd < 0) 
			error("ERROR on accept");
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		printf("Here is the message: %s\n",buffer);
		n = write(newsockfd,"I got your message",18);
		if (n < 0) error("ERROR writing to socket");
		//close all the socket connections
	}
	closeSocket(&newsockfd, &sockfd);
}


int createRoom()
{

}
