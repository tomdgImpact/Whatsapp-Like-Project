#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned int Hexa(char *bin)
{
    unsigned int b = strtoul(bin, NULL, 2);
    return b;
}

char *decToBinary(unsigned int n)
{
    char *b = malloc(64 * sizeof(char));
    for (int i = 0; i < 64; i++)
    {
        b[64 - i - 1] = n % 2 + '0';
        n = n / 2;
    }
    return b;
}

char *ToBinary(char msg)
{
    unsigned char c = msg;
    char *bin = calloc(9, sizeof(char));
    for (int i = 0; i < 8; ++i)
    {
        size_t remd = c % 2;
        c /= 2;
        bin[8 - i - 1] = remd + '0';
    }
    bin[8] = '\0';
    return bin;
}

char *hash(unsigned int h0, unsigned int h1, unsigned int h2, unsigned int h3, unsigned int h4)
{
    char *hash = malloc(160 * sizeof(char));
    snprintf(hash, 160, "%x%x%x%x%x", h0, h1, h2, h3, h4);
    return hash;
}

char *rev(char *b)
{
    char *r = malloc(64 * sizeof(char));
    for (int i = 0; i < 64; ++i)
        r[i] = b[64 - i - 1];

    return r;
}

char *padded(char *msg, long len)
{
    char *bl = rev(decToBinary(len * 8));
    char *pad = malloc(512 * sizeof(char));
    long bits = 0;
    long index = 0;
    char bool = 0;
    char *rbl = rev(bl);
    memset(pad, '0', 512);
    while (bits < 512)
    {
        if (bits <= 448)
        {
            if (index < len)
            {
                char *bin = ToBinary(msg[index]);
                for (long j = 0; j < 8; ++j)
                {
                    pad[bits] = bin[j];
                    if (j < 7)
                        bits++;
                }
                free(bin);
            }
            else if (bool == 0)
            {
                pad[bits] = '1';
                bool = 1;
            }
        }
        else
        {
            long q = 63;
            for (long j = 0; j < 64; ++j)
            {
                pad[512 - j - 1] = rbl[q];
                --q;
            }
        }
        ++bits;
        ++index;
    }
    //free(rbl);
    //free(pad);
    free(bl);
    return pad;
}

unsigned int intleftrotate(unsigned int x, unsigned int n)
{
    return (x << n) | (x >> (32 - n));
}

char *sha(char *msg)
{
    unsigned int h0 = 0x67452301;
    unsigned int h1 = 0xEFCDAB89;
    unsigned int h2 = 0x98BADCFE;
    unsigned int h3 = 0x10325476;
    unsigned int h4 = 0xC3D2E1F0;

    char *hashed = NULL;
    unsigned int div[80];
    long msg_len = strlen(msg);
    long std_len = 56;
    char *save = malloc(std_len * sizeof(char));
    int nb_chunk = 1;
    if (msg_len > std_len)
        nb_chunk = msg_len % std_len;
    for (long i = 0; i < nb_chunk; ++i)
    {
        memset(save, 0, std_len);
        for (int l = 0; l < msg_len && l < std_len; ++l)
        {
            save[l] = msg[l];
        }

        char *padded_ = padded(save, msg_len);
        char saved[32];
        for (int j = 0; j < 16; ++j)
        {
            for (int k = 0; k < 32; ++k)
            {
                saved[k] = *padded_++;
            }
            div[j] = Hexa(saved);
        }

        for (int j = 16; j < 80; ++j)
        {
            unsigned int hj3 = div[j - 3];
            unsigned int hj8 = div[j - 8];
            unsigned int hj14 = div[j - 14];
            unsigned int hj16 = div[j  - 16];

            unsigned int xor1 = hj3 ^ hj8;
            unsigned int xor2 = xor1 ^ hj14;
            unsigned int xor3 = xor2 ^ hj16;
            div[j] = intleftrotate(xor3, 1);
        }

        unsigned int a = h0;
        unsigned int b = h1;
        unsigned int c = h2;
        unsigned int d = h3;
        unsigned int e = h4;

        for (int s = 0; s < 80; ++s)
        {
            unsigned int f = 0;
            unsigned int k = 0;

            if (s <= 19)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (s >= 20 && s <= 39)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (s >= 40 && s <= 59)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            unsigned int tmp = intleftrotate(a, 5) + f + e + div[s] + k;

            e = d;
            d = c;
            c = intleftrotate(b, 30);
            b = a;
            a = tmp;
        }
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }
    hashed = hash(h0, h1, h2, h3, h4);
    return hashed;
}

int main(void)
{
    char *c = "voici un test de valeur de hash";
    char *hash = sha(c);
    printf("%s\n", hash);
    free(hash);
    printf("\n");
    return 0;
}