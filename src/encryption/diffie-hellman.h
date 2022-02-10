#ifndef DIFFIE_HELLMAN
#define DIFFIE_HELLMAN

typedef unsigned long long ull;

int Prime(ull a);

ull mod_pow(ull b, ull  exp, ull mod);

ull lenint(ull x);

unsigned char *strKey();

#endif