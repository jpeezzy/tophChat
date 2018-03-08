/**
 * Files contain general utilities like max, char to int
 * 
 */
#ifndef _UTILS_H
#define _UTILS_H

int max(int a, int b);

// slice including the start the last index
int stringSlicer(char *original, char *result, int start_index, int stop_index);

int charToInt(char character);

char intToChar(int number);

uniqueRandGen(int range, int *listPtr, int list_length);
#endif