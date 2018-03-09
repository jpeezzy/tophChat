#include <stdio.h>
#include <stdlib.h>


//This is an implementation of Suu Encrpytion
void encrypt(char input[500], unsigned long int key)
{
    int i,j;
    unsigned long int tempkey = key;
    unsigned long int subkey= tempkey%16;

    //15 rounds of encryption
    for(i = 0; i<15; ++i)
    {
        for(j=0; j<500; j++)
        {
            //NOT
            if(subkey % 2 == 1)
            {
                input[j] = ~input[j];
            }
            //XOR
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
    unsigned long int subkey;

    //15 rounds of decryption
    for(i = 0; i<15; ++i)
    {   
        if(i == 14)
        {
            tempkey = key;
        }
        else
        {
            tempkey = key/(16*(14-i));
        }
        subkey = tempkey%16;
        for(j=0; j<500; j++)
        {
            //Undo XOR 
            input[j] = input[j]^subkey;
            //Undo NOT
            if(subkey % 2 == 1)
            {
                input[j] = ~input[j];
            }
        }
    }

}

//for testing
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
    unsigned long int testkey = 64782395;
    printf(" %s \n", test);
    encrypt(test, testkey);
    printf(" %s\n",test);
    decrypt(test, testkey);
    printf(" %s\n", test);
    return 0;
}

