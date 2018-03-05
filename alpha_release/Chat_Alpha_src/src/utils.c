#include <assert.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

int max(int a, int b)
{
    return a >= b ? a : b;
}

// slice including the start and the last index
int stringSlicer(char *original, char *result, int start_index, int stop_index)
{
    unsigned int length = stop_index - start_index + 1;
    unsigned int i = 0;

    // handle copy till the end cases
    if(strlen(original)==(unsigned int)stop_index)
    {
        strcpy(result, original+start_index);
    }
    else
    {
    for (i = 0; i < length; ++i)
    {
        result[i] = original[start_index + i];
    }
    result[i] = '\0';
    }
    return 0;
}

int charToInt(char character)
{
    // values based on ASCII tables
    return (int)(character - 48);
}