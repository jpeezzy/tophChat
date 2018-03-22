#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "randGen.h"

//This is a random number generator based off the Issac Pesudo Random Generator 

unsigned long int randtable[256], randcount;
static unsigned long int mixer[256];
static unsigned long int am=0, bm = 0, cm = 0;

void SMA()
{
    unsigned long int i,temp,temp2;

    cm = cm + 1;
    bm = bm + cm ;

    for(i=0; i<256; ++i)
    {
        temp = mixer[i];
        switch(i%4)
        {
            case 0: am = am^(am<<13);
                    break;
            case 1: am = am^(am>>6);
                    break;
            case 2: am = am^(am<<2);
                    break;
            case 3: am = am^(am>>16);
                    break;
        }

        am = mixer[(i+128)%256] + am;
        mixer[i] = temp2 = mixer[(temp>>2)%256] + am + bm;
        randtable[i] = bm = mixer[(temp2>>10)%256] + temp;
    
    }

}

void mix(unsigned long int a, unsigned long int b, unsigned long int c, unsigned long int d, unsigned long int e, unsigned long int f, unsigned long int g, unsigned long int h) 
{
   a^=b<<11; d+=a; b+=c; 
   b^=c>>2;  e+=b; c+=d; 
   c^=d<<8;  f+=c; d+=e; 
   d^=e>>16; g+=d; e+=f; 
   e^=f<<10; h+=e; f+=g; 
   f^=g>>4;  a+=f; g+=h; 
   g^=h<<8;  b+=g; h+=a; 
   h^=a>>9;  c+=h; a+=b; 
}

void randInt(unsigned long int seed, int flag)
{
    int i;
    unsigned long int a,b,c,d,e,f,g,h;
    am=bm=cm=0;
    a=b=c=d=e=f=g=h=seed;

    for(i=0; i<4; ++i)
    {
        mix(a,b,c,d,e,f,g,h);
    }

    for(i=0; i<256; i+=8)
    {
        if(flag)
        {
            a+=randtable[i];
            b+=randtable[i+1];
            c+=randtable[i+2];
            d+=randtable[i+3];
            e+=randtable[i+4];
            f+=randtable[i+5];
            g+=randtable[i+6];
            h+=randtable[i+7];
        }

    }

    mix(a,b,c,d,e,f,g,h);
    mixer[i]=a;
    mixer[i+1]=b;
    mixer[i+2]=c;
    mixer[i+3]=d;
    mixer[i+4]=e;
    mixer[i+5]=f;
    mixer[i+6]=g;
    mixer[i+7]=h;

    if(flag)
    {
        for(i=0; i<256; i+=8)
        {
            a+=mixer[i];
            b+=mixer[i+1];
            c+=mixer[i+2];
            d+=mixer[i+3];
            e+=mixer[i+4];
            f+=mixer[i+5];
            g+=mixer[i+6];
            h+=mixer[i+7];
            mix(a,b,c,d,e,f,g,h);
            mixer[i]=a;
            mixer[i+1]=b;
            mixer[i+2]=c;
            mixer[i+3]=d;
            mixer[i+4]=e;
            mixer[i+5]=f;
            mixer[i+6]=g;
            mixer[i+7]=h;

        }
    }

    SMA();
    randcount=256;
}

unsigned long int randGen(unsigned long int SEED)
{
    unsigned long int i,j;
    am=bm=cm=(unsigned long int)0;
    for(i=0; i<256; ++i)
    {
        mixer[i]=randtable[i]=(unsigned long int)0;
    }
    randInt(SEED,1);
    for(i=0; i<2; ++i)
    {
        SMA();
        for(j=0; j<256; ++j)
        {
            //long n = strtol(randtable[j], NULL, 16);
            //printf("%.8lx",randtable[j]);
            //if((j&7)==7) printf("\n");
            return (unsigned long int)randtable[0];
        }

    }
    return 0;
}

unsigned long int randNum()
{
    int k;
    unsigned long int intial_seed = 0x9e3779b9;
    intial_seed = randGen(intial_seed);
    unsigned long int num;
    for(k=0; k<1; ++k)
    {
        intial_seed = num = randGen(intial_seed);
        return num;
        //printf("%lu \n",num);
    }
    return 0;

}
