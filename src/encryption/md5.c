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

char *hash(unsigned int h0, unsigned int h1, unsigned int h2, unsigned int h3)
{
    char *hash = malloc(128 * sizeof(char));
    snprintf(hash, 128, "%x%x%x%x", h0, h1, h2, h3);
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

char *md5(char *msg)
{
    unsigned int table[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 
                              5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 4, 
                              11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6,
                              10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    unsigned int k[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                           0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                           0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                           0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                           0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                           0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                           0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                           0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                           0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                           0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                           0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                           0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                           0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                           0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                           0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                           0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

    unsigned int a0 = 0x67452301;
    unsigned int a1 = 0xEFCDAB89;
    unsigned int a2 = 0x98BADCFE;
    unsigned int a3 = 0x10325476;

    char *hashed = NULL;
    unsigned int div[32];
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

        unsigned int a = a0;
        unsigned int b = a1;
        unsigned int c = a2;
        unsigned int d = a3;

        for (int s = 0; s <= 63; ++s)
        {
            unsigned int f = 0;
            unsigned int g = 0;

            if (s <= 15)
            {
                f = (b & c) | ((~b) & d);
                g = s;
            }
            else if (s >= 16 && s <= 31)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * s + 1) % 16;
            }
            else if (s >= 32 && s <= 47)
            {
                f = b ^ c ^ d;
                g = (3 * s + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * s) % 16;
            }
            f = f + a + k[s] + div[g];
            a = d;
            d = c;
            c = b;
            b = b + intleftrotate(f, table[s]);
        }
        a0 += a;
        a1 += b;
        a2 += c;
        a3 += d;
    }
    hashed = hash(a0, a1, a2, a3);
    return hashed;
}

int main(void)
{
    char *c = "The quick brown fox jumps over the lazy dog";
    char *hash = md5(c);
    printf("%s\n", hash);
    free(hash);
    printf("\n");
    return 0;
}
