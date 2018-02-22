/**
 * FIFO implementation used for messages entering a room
 * it will not overwrite data 
 */
#ifndef _FIFO_H
#define _FIFO_H

typedef struct FIFObuffer fifo;

struct FIFObuffer
{
    // use union if needing more types of buffer
    char **buffer;
    int bufLength;
    int readPos;
    int writePos;
};

struct FIFObuffer *initBuffer(int length);

char *readBuffer(fifo *buffer);

int writeBuffer(fifo *buffer, char *writeData, int length);

#endif
