#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pack.h"
#include "pack7.h"

static const char mapChar6Bit[64] = "\0ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890?";

static uint8_t charToBit6(char c)
{
    for (size_t i = 0; i < 64; ++i) {
        if (mapChar6Bit[i] == c)
            return i;
    }

    return 63;
}

static inline char bit6ToChar(uint8_t bit6)
{
    return mapChar6Bit[bit6];
}

void dumpHex(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", buf[i]);

        if (i % 8 == 7)
            printf("\n");
    }

    if (len % 8 != 0)
        printf("\n");
}

void test7Bit(void)
{
    const char str[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uint8_t buf[sizeof(str)] = {0};
    char unpack_buf[sizeof(str)] = "";

    printf("TEST: 7-bit packing\n");

    printf("Input string: %s\n", str);

    printf("Input HEX:\n");
    dumpHex((const uint8_t*)str, sizeof(str));

    size_t pack_len = sizeof(buf);
    bool ret = pack7Bit(str, strlen(str), buf, &pack_len);

    printf("Pack7 (ret = %d):\n", ret);
    dumpHex(buf, sizeof(buf));

    size_t unpack_len = sizeof(unpack_buf);
    ret = unpack7Bit(buf, pack_len, unpack_buf, &unpack_len);

    printf("Unpack7 HEX (ret = %d):\n", ret);
    dumpHex((const uint8_t*)unpack_buf, sizeof(unpack_buf));

    printf("Unpack7: %s\n", unpack_buf);

    if(strcmp(str, unpack_buf) == 0) {
        printf("--- OK ---\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
}

void mapString6Bit(char *str)
{
     while (*str) {
         *str = charToBit6(*str);
         str++;
     }
}

void unmapString6Bit(char *str)
{
    while (*str) {
        *str = bit6ToChar(*str);
        str++;
    }
}

void test6Bit(void)
{
    const uint8_t BITS = 6;

    const char str[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char preparedStr[sizeof(str)] = "\0";

    uint8_t buf[sizeof(str)] = {0};
    char unpackBuf[sizeof(str)] = "\0";

    printf("TEST: arbitrary packing (6-bit)\n");

    printf("Input string: %s\n", str);

    printf("Input HEX:\n");

    dumpHex((const uint8_t*)str, sizeof(str));

    strcpy(preparedStr, str);

    mapString6Bit(preparedStr);

    size_t packLen = sizeof(buf);
    bool ret = packBits((const uint8_t*)preparedStr, strlen(preparedStr), buf, &packLen, BITS);

    printf("Pack6 (ret = %d):\n", ret);

    dumpHex(buf, sizeof(buf));

    size_t unpackLen = sizeof(unpackBuf);
    ret = unpackBits(buf, packLen, unpackBuf, &unpackLen, BITS);

    printf("Unpack6 HEX (ret = %d):\n", ret);
    dumpHex((const uint8_t*)unpackBuf, sizeof(unpackBuf));

    unmapString6Bit((char*)unpackBuf);

    printf("Unpack6: %s\n", unpackBuf);

    if(strcmp(str, (const char*)unpackBuf) == 0) {
        printf("--- OK ---\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
}

int main(int argc, char *argv[])
{
    test7Bit();
    test6Bit();

    return 0;
}
