#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool pack7bit(const char *str, uint8_t *const out, size_t *const out_len)
{
    const size_t len = strlen(str);
    size_t pack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t max_out_len = *out_len;

    if (len < ((max_out_len * 7) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        uint8_t byte = (str[i] & 0x7F);

        uint8_t toPush = 0;

        const uint8_t lsh_bits = (8 - carryBitsCount);
        const uint8_t mask = 0xFFU >> lsh_bits;
        const uint8_t mask2 = 0xFFU >> carryBitsCount;

        carryBits &= mask;
        carryBits |= (byte & mask2) << carryBitsCount;

        if (carryBitsCount == 0) {
            toPush = 0;
            carryBitsCount = 7;
        } else if (carryBitsCount <= 8) {
            toPush = carryBitsCount - 1;
            carryBitsCount = 8;
        } else {
            assert(0);
        }

        if (carryBitsCount == 8) {
            out[pack_len] = carryBits;
            ++pack_len;
            carryBitsCount = 0;

            const uint8_t lsh_bits = (8 - (toPush + 1));
            const uint8_t mask = 0xFFU >> lsh_bits;

            if (toPush <= 7) {
                carryBits = (byte >> lsh_bits) & mask;
                carryBitsCount = toPush;
            } else {
                assert(0);
            }
        }
    }

    if (carryBitsCount > 0) {
        out[pack_len] = carryBits;
        ++pack_len;
    }

    *out_len = pack_len;

    return true;
}

bool unpack7bit(const uint8_t *str7bit, size_t len, char *out, size_t *out_len)
{
    size_t  unpack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;

    size_t max_out_len = *out_len;

    if (max_out_len < ((len * 8)/ 7))
        return false;

    for (size_t i = 0; i < len; ++i) {
        const uint8_t byte = str7bit[i];
        uint8_t toPush = 0;

        const uint8_t mask  = 0xFFU >> (8-carryBitsCount);
        const uint8_t mask2 = 0xFFU >> carryBitsCount;

        if (carryBitsCount <= 8) {
            carryBits &= mask;
            carryBits |= (byte & mask2) << carryBitsCount;
            toPush = carryBitsCount;
            carryBitsCount = 8;
        } else {
            assert(0);
        }


        out[unpack_len] = (carryBits & 0x7F);
        ++unpack_len;

        carryBits >>= 7;
        carryBitsCount -= 7;

        if (toPush <= 8) {
            carryBits |= (byte >> (8 - toPush)) << 1;
            if (toPush == 8) {
                out[unpack_len] = (carryBits & 0x7F);
                ++unpack_len;
                carryBits = carryBits >> 7;
                carryBits |= (byte >> 7) << 1;
                carryBitsCount = 2;
            } else {
                carryBitsCount += toPush;
            }
        } else {
            assert(0);
        }
    }

    if (carryBitsCount > 0) {
        if (carryBitsCount == 8) {
            out[unpack_len] = (carryBits & 0x7F);
            out[unpack_len+1] = (carryBits >> 7) & 0x01;
        }
        else {
            out[unpack_len] = carryBits & 0x7F;
        }
    }

    *out_len = unpack_len;

    return true;
}

void dumphex(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", buf[i]);

        if (i % 8 == 7)
            printf("\n");
    }

    if(len % 8 != 0)
        printf("\n");
}

void test(void)
{
    const char str[] = "~{}~{}[][]ABCDEFGH[][]~{}~{}abcdefgh~{}~{}1234567890~{}~{}";
    uint8_t buf[sizeof(str)] = {0};
    char unpack_buf[sizeof(str)] = "";
    printf("Input string: %s\n", str);

    printf("Input HEX:\n");
    dumphex((const uint8_t*)str, sizeof(str));

    size_t pack_len = sizeof(buf);
    bool ret = pack7bit(str, buf, &pack_len);

    printf("Pack7 (ret = %d):\n", ret);
    dumphex(buf, sizeof(buf));

    size_t unpack_len = sizeof(unpack_buf);
    ret = unpack7bit(buf, pack_len, unpack_buf, &unpack_len);

    printf("Unpack7 HEX (ret = %d):\n", ret);
    dumphex((const uint8_t*)unpack_buf, sizeof(unpack_buf));

    printf("Unpack7: %s\n", unpack_buf);

    if(strcmp(str, unpack_buf) == 0) {
        printf("--- OK ---\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
}

int main(int argc, char *argv[])
{
    test();
    return 0;
}
