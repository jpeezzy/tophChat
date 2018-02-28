#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "fifo.h"
#include "constants.h"


// read buffer and pop the element index 0
struct FIFObuffer *initBuffer(int length)
{
    assert(length);

    fifo *temp = malloc(sizeof(struct FIFObuffer));
    assert(temp);
    temp->bufLength = length;
    temp->readPos = 0;
    temp->writePos = 0;

    // initialize all pointer with NULL
    for (int i = 0; i < length; ++i)
    {
        (temp->buffer) = NULL;
    }

    return temp;
}

// return NULL if there is nothing to read
char *readBuffer(fifo *buffer)
{
    assert(buffer);
    assert(buffer->bufLength);

    char *temp;

    // if elements had been read or had not been written to, return NULL
    if ((temp = (buffer->buffer)[buffer->readPos]) == NULL)
    {
        return NULL;
    }
    else
    {
        // empty the buffer that was read
        free(buffer->buffer[buffer->readPos]);
        buffer->buffer[buffer->readPos] = NULL;
    }

    // wrap around if reaching the end of buffer
    buffer->readPos = (buffer->readPos + 1) % (buffer->bufLength);

    return temp;
}

// write to the first free position (prioritize larger index)
// will malloc new data to copy the writeData to
int writeBuffer(fifo *buffer, char *writeData, int length)
{
    assert(buffer);
    assert(buffer->bufLength);
    assert(writeData);

    char *temp = malloc(sizeof(char) * length);

    // will only write if there is no data there
    if ((buffer->buffer)[buffer->writePos] == NULL)
    {
        (buffer->buffer)[buffer->writePos] = temp;
        buffer->writePos = (buffer->writePos + 1) % (buffer->bufLength);
    }
    else
    {
        return FIFO_FULL;
    }

    return 0;
}

#ifdef FIFODEBUG

void printBuffer(fifo *buffer)
{
}

int main(void)
{
    return 0;
}

#endif