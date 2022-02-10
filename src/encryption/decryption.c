#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "genkey.h"
#include "decryption.h"
#include <stdlib.h>
#include <stdint.h>
uint8_t _gmult(uint8_t a, uint8_t b);
#define DEBUG_ARR_LN(from, to, ptr)\
    for (size_t i = (from); i < (to); i++)\
        printf(" %x ", (ptr)[i]);

unsigned char S_box[256] =
{
    0x52, 0x09,	0x6A, 0xD5,	0x30, 0x36,	0xA5, 0x38,	0xBF, 0x40,	0xA3, 0x9E,	0x81, 0xF3,	0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xa2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xb6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xfC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D,
};

unsigned char inv_matrix[16] =
{
    0x0E, 0x0B, 0x0D, 0x09,
    0x09, 0x0E, 0x0B, 0x0D,
    0x0D, 0x09, 0x0E, 0x0B,
    0x0B, 0x0D, 0x09, 0x0E,
};

unsigned char Rcon[16] =
{
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
};

void InvSubByte(unsigned char* state)
{
    for (int i = 0; i < 16; i++)
        state[i] = S_box[state[i]];
}

void InvShiftRows(unsigned char* state)
{
    unsigned char tmp[16];
    tmp[0] = state[0];
    tmp[1] = state[13];
    tmp[2] = state[10];
    tmp[3] = state[7];
    tmp[4] = state[4];
    tmp[5] = state[1];
    tmp[6] = state[14];
    tmp[7] = state[11];
    tmp[8] = state[8];
    tmp[9] = state[5];
    tmp[10] = state[2];
    tmp[11] = state[15];
    tmp[12] = state[12];
    tmp[13] = state[9];
    tmp[14] = state[6];
    tmp[15] = state[3];

    for (int i = 0; i < 16; i++)
        state[i] = tmp[i];
}

void InvMixColumns(unsigned char state[])
{
    unsigned char tmp[16];

    tmp[0] = _gmult(0x0E, state[0]) ^ _gmult(0x0B, state[1]) ^ _gmult(0x0D, state[2]) ^ _gmult(0x09, state[3]);
    tmp[1] = _gmult(0x09, state[0]) ^ _gmult(0x0E, state[1]) ^ _gmult(0x0B, state[2]) ^ _gmult(0x0D, state[3]);
    tmp[2] = _gmult(0x0D, state[0]) ^ _gmult(0x09, state[1]) ^ _gmult(0x0E, state[2]) ^ _gmult(0x0B, state[3]);
    tmp[3] = _gmult(0x0B, state[0]) ^ _gmult(0x0D, state[1]) ^ _gmult(0x09, state[2]) ^ _gmult(0x0E, state[3]);

    tmp[4] = _gmult(0x0E, state[4]) ^ _gmult(0x0B, state[5]) ^ _gmult(0x0D, state[6]) ^ _gmult(0x09, state[7]);
    tmp[5] = _gmult(0x09, state[4]) ^ _gmult(0x0E, state[5]) ^ _gmult(0x0B, state[6]) ^ _gmult(0x0D, state[7]);
    tmp[6] = _gmult(0x0D, state[4]) ^ _gmult(0x09, state[5]) ^ _gmult(0x0E, state[6]) ^ _gmult(0x0B, state[7]);
    tmp[7] = _gmult(0x0B, state[4]) ^ _gmult(0x0D, state[5]) ^ _gmult(0x09, state[6]) ^ _gmult(0x0E, state[7]);

    tmp[8] = _gmult(0x0E, state[8]) ^ _gmult(0x0B, state[9]) ^ _gmult(0x0D, state[10]) ^ _gmult(0x09, state[11]);
    tmp[9] = _gmult(0x09, state[8]) ^ _gmult(0x0E, state[9]) ^ _gmult(0x0B, state[10]) ^ _gmult(0x0D ,state[11]);
    tmp[10] = _gmult(0x0D, state[8]) ^ _gmult(0x09, state[9]) ^ _gmult(0x0E, state[10]) ^ _gmult(0x0B, state[11]);
    tmp[11] = _gmult(0x0B, state[8]) ^ _gmult(0x0D, state[9]) ^ _gmult(0x09, state[10]) ^ _gmult(0x0E, state[11]);

    tmp[12] = _gmult(0x0E, state[12]) ^ _gmult(0x0B, state[13]) ^ _gmult(0x0D, state[14]) ^ _gmult(0x09, state[15]);
    tmp[13] = _gmult(0x09, state[12]) ^ _gmult(0x0E, state[13]) ^ _gmult(0x0B, state[14]) ^ _gmult(0x0D, state[15]);
    tmp[14] = _gmult(0x0D, state[12]) ^ _gmult(0x09, state[13]) ^ _gmult(0x0E, state[14]) ^ _gmult(0x0B, state[15]);
    tmp[15] = _gmult(0x0B, state[12]) ^ _gmult(0x0D, state[13]) ^ _gmult(0x09, state[14]) ^ _gmult(0x0E, state[15]);

    for (int i = 0; i < 16; i++)
        state[i] = tmp[i];
}

void AddRoundkey(unsigned char* state, unsigned char* RoundKey)
{
    for (int i = 0; i < 16; i++)
        state[i] ^= RoundKey[i];
}

void AES_decryption(unsigned char* message, unsigned char* key)
{
    unsigned char state[16];
    for (int i = 0; i < 16; i++)
        state[i] = message[i];

    unsigned char expandedKey[176];

    KeyExpansion(key, expandedKey);

    AddRoundkey(state, expandedKey + 160);
    InvShiftRows(state);
    InvSubByte(state);
    /*for (int i = 0; i < 16; i++)
    {
        printf("%x ", state[i]);
    }
    printf("\n");*/

    for (int i = 9; i > 0; i--)
    {
        AddRoundkey(state, expandedKey + (16 * (i + 1)));
        InvMixColumns(state);
        InvShiftRows(state);
        InvSubByte(state);
        /*for (int i = 0; i < 16; i++)
        {
            printf("%x ", state[i]);
        }
        printf("\n");*/
    }

    AddRoundkey(state, key);
    /*printf("Test: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%x ", state[i]);
    }
    printf("\n");*/

    for (int i = 0; i < 16; i++)
        message[i] = state[i];
}

void printhex(unsigned char x)
{
    if (x / 16 < 10)
        printf("%c", (x / 16) + '0');
    if (x / 16 >= 10)
        printf("%c", (x / 16 - 10) + 'A');
    if (x % 16 < 10)
        printf("%c", (x % 16) + '0');
    if (x % 16 >= 10)
        printf("%c", (x % 16 - 10) + 'A');
}

uint8_t _gmult(uint8_t a, uint8_t b) 
{
	uint8_t p = 0;
	while (a && b) {
            if (b & 1)
                p ^= a;

            if (a & 0x80)
                a = (a << 1) ^ 0x11b;
            else
                a <<= 1;
            b >>= 1;
	}
	return p;
}


unsigned char *decrypt(unsigned char *message ,unsigned char *key_str)
{
    // unsigned char message[] = "B64B27BB1615A6F532186CC5FA94B55E5C54EA1BDF971E3DE31BFC0275227652D57BD542BA0F6850CDFD59B8EB0E83D1";
    /*unsigned char key_str[16] =
    {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16,
    };*/

    AES_decryption(message, key_str);

    printf("Decrypted message: \n");
    size_t len = strlen((char *) message);
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", message[i]);
    }
    printf("\n");
    printf("Length of the message = %lu\n", len);

    return message;
}

unsigned char *hex_str_to_ascii_str(unsigned char *hex_str, int *length_ascii)
{
    unsigned char *ascii_str;
    size_t j;

    assert(strlen((const char *) hex_str) % 2 == 0);
    *length_ascii = strlen((const char *) hex_str) / 2;
    ascii_str = malloc((*length_ascii) * sizeof(unsigned char));

    j = 0;

    for (size_t i = 0; i < strlen((const char *) hex_str); i += 2)
    {
        int first_digit;
        int second_digit;

        if (hex_str[i] > '9')
            first_digit = hex_str[i] - 'A' + 10;
        else
            first_digit = hex_str[i] - '0';


        if (hex_str[i+1] > '9')
            second_digit = hex_str[i+1] - 'A' + 10;
        else
            second_digit = hex_str[i+1] - '0';

        ascii_str[j] = first_digit * 16 + second_digit;
        j += 1;
    }

    return ascii_str; // to be freed
}

/*int main(void)
{
    unsigned char message[] = "69C4E0D86A7B0430D8CDB78070B4C55A";
    // unsigned char message[] = "B64B27BB1615A6F532186CC5FA94B55E5C54EA1BDF971E3DE31BFC0275227652D57BD542BA0F6850CDFD59B8EB0E83D1";

    int originalLen;

    unsigned char *str_message = hex_str_to_ascii_str(message, &originalLen);

    unsigned char key_str[16] =
    {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16,
    };

    printf("%i\n", originalLen);

    int lenOfPaddedMessage = originalLen;

    if (lenOfPaddedMessage % 16 != 0)
        lenOfPaddedMessage = (lenOfPaddedMessage / 16 + 1) * 16;

    //unsigned char paddedMessage[lenOfPaddedMessage];
    unsigned char *paddedMessage = calloc(lenOfPaddedMessage, sizeof(char));
    for (int i = 0; i < lenOfPaddedMessage; i++)
    {
        if (i >= originalLen)
            paddedMessage[i] = 0;
    }

    for (int i = 0; i < lenOfPaddedMessage; i += 16)
        AES_decryption(paddedMessage + i, key_str);

    // unsigned char *res = decrypt(str_message, key_str);
    /i*for (size_t i = 0; i < strlen(res); ++i)
    {
        printhex(res[i]);
    }
    return 0;
}*/
