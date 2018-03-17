#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#define bufsize 20
int main()
{
	printf("initializing buffer. \n");
	fifo *test = initBuffer(20);
	printf("Buffer Initialized!. \n");
	printf("\nStarting writing buffer\n");
	for(int i = 0; i < bufsize; i++)
	{
		writeBuffer(test, "HELLO");
	}
	printf("Finished writing buffer\n");
	printf("\nStarting readBuffer\n");
	char* charpointer = malloc(sizeof(char)*300);
	
	for(int i = 0; i < bufsize; i++)
	{
		bzero(charpointer, sizeof(charpointer));
		readBuffer(test, charpointer);
		printf("word is %s \n", charpointer);
	}
	free(charpointer);
	printf("Finished readBuffer!\n");
	closeBuffer(test);
	return 0;
}
