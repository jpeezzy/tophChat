#include "utils.h"

int max(int a, int b)
{
    return a >= b ? a : b;
}

// slice including the start the last index
int stringSlicer(char *original, char *result, int start_index, int stop_index)
{
    int length = stop_index - start_index + 1;
    int i = 0;
    assert(strlen(result) >= length);
    for (i = 0; i < length; ++i)
    {
        result[i] = original[start_index + i];
    }
    result[i] = '\0';
}

int charToInt(char character)
{
    // values based on ASCII tables
    return (int)(character - 48);
}