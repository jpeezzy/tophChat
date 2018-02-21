#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include "transportClient.h"

int main(void)
{   
    struct addrinfo* serverInfo;
    struct addrinfo hints;
    hints.ai_family=AF_INET;

    getaddrinfo("zuma.eecs.uci.edu", NULL, &hints, &serverInfo);

    struct addrinfo* temp=NULL;
    char host[100];
    for(temp=serverInfo; temp!=NULL; temp=temp->ai_next)
    {
        getnameinfo(temp->ai_addr, temp->ai_addrlen, host, (socklen_t) sizeof(host), NULL,0,NI_NUMERICHOST);
        puts(host);
    }
    freeaddrinfo(serverInfo);
    return 0;
}
