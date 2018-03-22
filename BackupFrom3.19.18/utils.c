#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    memset(result, '\0', sizeof(char) * length);
    // strncpy(result, original + start_index, sizeof(char) * length);

    // printf("parsing for string\n");
    for(int i=0; i<length; ++i)
    {
        result[i]=original[start_index+i];
        printf("%c",result[i]);
    }
    //Check if socket is using up all the chars if so, add null
    //terminator to overrite lats charactre. if not, just append null
    //terminator
    //  printf("\n");
    //  printf("\nthe result is%s for string slicer\n", result);
    int nullTerminator = (strlen(result) < PACKAGE_SIZE - 1) ? start_index + length : start_index + length - 1;
    result[nullTerminator] = '\0';
    // for (i = 0; i < length; ++i)
    // {
    //     result[i] = original[start_index + i];
    // }
    // result[i - 1] = '\0';
    return 0;
}

int charToInt(char character)
{
    // values based on ASCII tables
    return (int)(character - 48);
}

char intToChar(int number)
{
    return (char)(number + 48);
}

// assume that the supplied string has space for the number and the null terminator
void intToString(int number, char *numberString, int numLength)
{
    int i = 0;
    for (i = 0; i < numLength; ++i)
    {
        numberString[numLength - i - 1] = intToChar(number % 10);
        number /= 10;
    }
    numberString[i] = '\0';
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

// #ifdef DEBUG
// int main()
// {
// 	char* test= "3.1415926535 8979323846 2643383279 5028841971 6939937510 5820974944 5923078164 0628620899 8628034825 3421170679 8214808651 3282306647 0938446095 5058223172 5359408128 4811174502 8410270193 8521105559 6446229489 5493038196 4428810975 6659334461 2847564823 3786783165 2712019091 4564856692 3460348610 4543266482 1339360726 024914127";
// 	char* result = malloc(sizeof(char)*30);
// 	stringSlicer(test, result, 0,200);
// 	printf("%s\n", result);
// 	return 0;
// }
// #endif
