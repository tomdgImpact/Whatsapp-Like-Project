#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "diffie-hellman.h"
typedef unsigned long long ull;

// generate 2 keys with 8 digits each
// make a function to make sure that the result is 8 digits long
// apply the function to the server

ull mod_pow(ull b, ull  exp, ull mod)
{
    if (mod == 1)
        return 0;
    ull result = 1;
    b %= mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)//Prime(exp))
        {
            result = (result * b) % mod;
        }
        exp >>= 1;
        b = (b * b) % mod;
    }
    return result;
}

int Prime(ull a)
{
   ull c;
 
   for (c = 2; c <= a - 1; c++)
   { 
      if (a % c == 0)
        return 0;
   }
   return 1;
}

ull lenint(ull x)
{
    ull len;
    for (len = 0; x > 0; ++len, x /= 10)
        continue;
    
    return len;
}

unsigned char *strKey(ull nb)
{
    unsigned char *str_key = malloc(16 * sizeof(unsigned char));

    for (size_t i = 0; i < 16; i++)
    {
        str_key[i] = nb % 10;
        nb /= 10;
    }
    return str_key;
}

/*int main()
{
    test_nb();
    return 0;
}*/

