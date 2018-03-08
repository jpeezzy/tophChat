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
int closeBuffer(fifo *buf);

int readBuffer(fifo *buf_struct, char *buf_content);

int writeBuffer(fifo *buffer, const char *writeData);

#endif
