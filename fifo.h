#ifndef _FIFO_H
#define _FIFO_H

typedef struct FIFObuffer fifo;

struct FIFObuffer
{
    char* buffer[20];
    int curPos; 
};

int readBuffer(fifo* buffer);

int writeBuffer(fifo* buffer);

#endif 



