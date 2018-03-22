#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//This is an implementation of Suu Encrpytion
void encrypt(char input[500], unsigned long int key)
{
    int i,j;
    unsigned long int tempkey = key;
    unsigned long int subkey= tempkey%16;

    //7 rounds of encryption
    for(i = 0; i<1; ++i)
    {
        for(j=0; j<499; j++)
        {
            //XOR
            input[j] = input[j]^subkey;
            //NOT
            input[j] = ~input[j];
        }
        tempkey = tempkey/16;
        subkey = tempkey%16;
    }

}

void decrypt(char input[500], unsigned long int key)
{

    int i,j;
    unsigned long int tempkey = key;
    unsigned long int subkey;

    //7 rounds of decryption
    for(i = 0; i<1; ++i)
    {   
        if(i == 6)
        {
            tempkey = key;
        }
        else
        {
            tempkey = key/(16*(6-i));
        }
        subkey = key%16;
        for(j=0; j<499; j++)
        {
            //NOT
            input[j] = ~input[j];
            //XOR
            input[j] = input[j]^subkey;
        }
    }

}

#ifdef MAIN
int main()
{
    char test[500];
    int i;
    for(i=0; i<500; ++i)
    {
        test[i] = ' ';
    }
    test[0] = 'A';
    test[1] = 'b';
    test[2] = 'e';
    test[3] = 'l';
    test[4] = ' ';
    test[5] = 'i';
    test[6] = 's';
    test[7] = ' ';
    test[8] = 't';
    test[9] = 'h';
    test[10] = 'e';
    test[499] = '\0';
    unsigned long int testkey = 0x1111111ff;
    printf(" %s \n", test);
    encrypt(test, testkey);
    printf(" %s\n",test);
    decrypt(test, testkey);
    printf(" %s\n", test);
    return 0;
}

#endif
