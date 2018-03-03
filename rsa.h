#ifndef RSA_H
#define RSA_H

typedef struct rsa_priv_keys RSA_PRIV;
typedef struct rsa_pub_keys RSA_PUB;

struct rsa_priv_keys
{
    unsigned long n;
    unsigned long d;
};

struct rsa_pub_keys
{
    unsigned long n;
    unsigned long e;
};

RSA_PRIV *createRSA_priv(unsigned long n, unsigned long d);

RSA_PUB *createRSA_pub(unsigned long n, unsigned long e);

void deleteRSA_priv(RSA_PRIV *rsa_priv);

void deleteRSA_pub(RSA_PUB *rsa_pub);

/* generate a 1024 bit prime number p for rsa encryption 
 * where p must satisfy the condition (p mod e) != 1 */
unsigned long generateRSA_prime(unsigned long e);

int testPrime(unsigned long p);

int millerTest(unsigned long p, unsigned long d);

unsigned long power(unsigned long x, unsigned long n);

unsigned long mod_mul(unsigned long x, unsigned long y, unsigned long n);

unsigned long mod_exp(unsigned long b, unsigned long e, unsigned long n);

unsigned long ext_gcd(unsigned long a, unsigned long b, long *x, long *y);

unsigned long mod_inv(unsigned long a, unsigned long b);

void generateRSA_Keys(RSA_PRIV* rsaPriv, RSA_PUB* rsaPub);

/* encrypts message into cipher */
unsigned long rsa_encrypt(unsigned long message, RSA_PUB* rsaPub);

/* decrypts cipher into message */
unsigned long rsa_decrypt(unsigned long cipher, RSA_PRIV* rsaPriv);

#endif

