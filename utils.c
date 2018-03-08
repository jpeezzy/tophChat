#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    if (strlen(original) == (unsigned int)stop_index)
    {
        strcpy(result, original + start_index);
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

char intToChar(int number)
{
    return (char)number + 48;
}

int uniqueRandGen(int range, int *listPtr, int list_length)
{
    srand(time(NULL));
    for (int i = 0; i < list_length; ++i)
    {
    randGen_1:
        listPtr[i] = rand() % (range + 1);

        for (int j = 0; j < i; ++j)
        {
            if (listPtr[j] == listPtr[i])
            {
                goto randGen_1;
            }
        }
    }
    return 0;
}