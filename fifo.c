#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "fifo.h"
#include "constants.h"

fifo *initBuffer(int length)
{
    assert(length);

    fifo *temp = (fifo *)malloc(sizeof(struct FIFObuffer));
    assert(temp);
    temp->bufLength = length;
    temp->readPos = 0;
    temp->writePos = 0;
    temp->buffer = (char **)malloc(sizeof(char *) * length);
    // initialize all pointer with NULL
    for (int i = 0; i < length; ++i)
    {
        (temp->buffer)[i] = NULL;
    }

    return temp;
}

int closeBuffer(fifo *buf)
{
    assert(buf);
    for (int i = 0; i < buf->bufLength; ++i)
    {
        free(buf->buffer[i]);
    }
    free(buf->buffer);
    free(buf);
    return 0;
}

int readBuffer(fifo *buf_struct, char *buf_content)
{
    // printf("in read buffer\n");
    assert(buf_struct);
    assert(buf_struct->bufLength);
#ifdef DEBUG
    printf("passes assertions in readBuffer");
#endif
    if ((buf_struct->buffer)[buf_struct->readPos] == NULL)
    {
        return FIFO_NO_DATA;
    }
    else
    {
        memcpy(buf_content, (buf_struct->buffer)[buf_struct->readPos], sizeof(char) * PACKAGE_SIZE);
#ifdef DEBUG
        printf("\nbuffer content is %s\n", (buf_struct->buffer)[buf_struct->readPos]);
#endif
        free((buf_struct->buffer)[buf_struct->readPos]);
#ifdef DEBUG
        printf("\nfreed buffer content %s\n", buf_content);
#endif
        buf_struct->buffer[buf_struct->readPos] = NULL;
    }
    // wrap around if reaching the end of buffer
    buf_struct->readPos = (buf_struct->readPos + 1) % (buf_struct->bufLength);
    return 0;
}

// write to the first free position (prioritize larger index)
// will malloc new data to copy the writeData to
// assume all things are in string form
int writeBuffer(fifo *buf, const char *writeData)
{
    assert(buf);
    assert(buf->bufLength);
    assert(writeData);

    // will only write if there is no data there
    if ((buf->buffer)[buf->writePos] == NULL)
    {
        char *temp = (char *)calloc(PACKAGE_SIZE + 2, sizeof(char));
        memcpy(temp, writeData, sizeof(char) * (PACKAGE_SIZE));
        (buf->buffer)[buf->writePos] = temp;
        buf->writePos = (buf->writePos + 1) % (buf->bufLength);
    }
    else
    {
        return FIFO_FULL;
    }

    return 0;
}
