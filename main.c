#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pack.h"
#include "pack7.h"

//#define ENABLE_LOG_DUMP // Uncomment to get string and hex dumps
#define MAX_LEN 128

static const char mapChar6Bit[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890?";

static const char mapChar5Bit[32] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde";

static const char mapChar4Bit[64] = "0123456789ABCDEF";

static const char mapChar3Bit[] = "01234567";

static const char mapChar2Bit[] = "0123";

static const char mapChar1Bit[] = "23";

void dumpHex(const char *prefixStr, const uint8_t *buf, size_t len)
{
    (void)prefixStr;
    (void)buf;
    (void)len;
#ifdef ENABLE_LOG_DUMP
    printf("%s HEX:\n", prefixStr);

    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", buf[i]);

        if (i % 8 == 7)
            printf("\n");
    }

    if (len % 8 != 0)
        printf("\n");
#endif
}

void dumpString(const char *prefixStr, const char *str)
{
    (void)prefixStr;
    (void)str;
#ifdef ENABLE_LOG_DUMP
    printf("%s:%s\n", prefixStr, str);
#endif
}

static uint8_t charToBit(char c, const char *charMap, uint8_t bits)
{
    const uint8_t maxLen = 1U << bits;

    if (strlen(charMap) < maxLen - 1U)
        return maxLen;

    for (size_t i = 0; i <= maxLen; ++i) {
        if (charMap[i] == c)
            return i;
    }

    return maxLen;
}

static inline char bitToChar(uint8_t pos, const char *charMap)
{
    return charMap[pos];
}

void mapStringBit(char *str, const char *charMap, uint8_t bits)
{
    while (*str) {
        *str = charToBit(*str, charMap, bits);
        str++;
    }
}

void unmapStringBit(char *str, size_t len, const char *charMap)
{
    for (size_t i = 0; i < len; ++i) {
        *str = bitToChar(*str, charMap);
        str++;
    }
}

void test7Bit(void)
{
    const char str[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    uint8_t buf[sizeof(str)] = {0};
    char unpack_buf[sizeof(str)] = "";

    printf("TEST: 8to7 packing\n");

    dumpString("Input string", str);

    dumpHex("Input", (const uint8_t*)str, sizeof(str));

    size_t pack_len = sizeof(buf);
    bool ret = pack7Bit(str, strlen(str), buf, &pack_len);
    if (ret)
        dumpHex("Pack7", buf, sizeof(buf));
    else
        printf("Pack7 ERROR\n");

    size_t unpack_len = sizeof(unpack_buf);
    ret = unpack7Bit(buf, pack_len, unpack_buf, &unpack_len);

    if (ret)
        dumpHex("Unpack7", (const uint8_t*)unpack_buf, sizeof(unpack_buf));
    else
        printf("Unpack7 ERROR\n");

    dumpString("Unpack7", unpack_buf);

    if(strcmp(str, unpack_buf) == 0) {
        printf("--- OK ---\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
}

void testBits(uint8_t bits, const char *str, const char *charMap)
{
    char preparedStr[MAX_LEN] = "";
    char packStr[16];
    char unpackStr[16];

    const size_t strsize = strlen(str) + 1;

    uint8_t buf[MAX_LEN] = {0};
    uint8_t unpackBuf[MAX_LEN] = {0};

    printf("TEST: arbitrary packing (%u-bit)\n", bits);

    snprintf(packStr, sizeof(packStr), "Pack%u", bits);
    snprintf(unpackStr, sizeof(unpackStr), "Unpack%u", bits);

    dumpString("Input string", str);

    dumpHex("Input", (const uint8_t*)str, strsize);

    strcpy(preparedStr, str);

    const size_t strLen = strlen(preparedStr);

    if (charMap)
        mapStringBit(preparedStr, charMap, bits);

    size_t packLen = sizeof(buf);
    bool ret = packBits((const uint8_t*)preparedStr, strLen, buf, &packLen, bits);

    if (ret)
        dumpHex(packStr, buf, sizeof(buf));
    else
        printf("%s ERROR\n", packStr);

    size_t unpackLen = sizeof(unpackBuf);
    ret = unpackBits(buf, packLen, unpackBuf, &unpackLen, bits);

    if (ret)
        dumpHex(unpackStr, (const uint8_t*)unpackBuf, sizeof(unpackBuf));
    else
        printf("%s ERROR\n", unpackStr);

    if (charMap)
        unmapStringBit((char*)unpackBuf, unpackLen, charMap);

    unpackBuf[strsize-1] = '\0';

    dumpString(unpackStr, (char*)unpackBuf);

    if(strcmp(str, (const char*)unpackBuf) == 0) {
        printf("--- OK ---\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
}

int main(int argc, char *argv[])
{
    const char str7[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char str6[] = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char str5[] = "ABCDEFIJKLMNOPQRSTUVWXYZabcdeABCDEF";
    const char str4[] = "ABCDEF0123456789";
    const char str3[] = "012345677776543210";
    const char str2[] = "012332101";
    const char str1[] = "23322233";

    test7Bit();
    testBits(7, str7, NULL);
    testBits(6, str6, mapChar6Bit);
    testBits(5, str5, mapChar5Bit);
    testBits(4, str4, mapChar4Bit);
    testBits(3, str3, mapChar3Bit);
    testBits(2, str2, mapChar2Bit);
    testBits(1, str1, mapChar1Bit);

    return 0;
}
