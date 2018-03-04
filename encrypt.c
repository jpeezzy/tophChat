#include <stdio.c>
#include <stdlib.c>


//This is an implementation of Suu Encrpytion
void encrypt(char input[500], unsigned long int key)
{
    int i,j;
    unsigned long int tempkey = key;
    unsigned long int subkey= tempkey%16;
    tempkey = tempkey/16;

    //15 rounds of encryption
    for(i = 0; i<15; ++i)
    {
        for(j=0; j<500; j++)
        {
            input[j] = input[j]^subkey;
        }
        tempkey = tempkey/16;
        subkey = tempkey%16;
    }

}

void decrypt(char input[500], unsigned long int key)
{

    int i,j;
    unsigned long int tempkey = key;
    unsigned long int subkey= tempkey%16;
    tempkey = tempkey/16;

    //15 rounds of decryption
    for(i = 0; i<15; ++i)
    {
        for(j=0; j<500; j++)
        {
            input[j] = input[j]^subkey;
        }
        tempkey = tempkey/(16*(15-i));
        subkey = tempkey%16;
    }

}
