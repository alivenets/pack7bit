#include <assert.h>
#
#include "pack7.h"

bool pack7Bit(const char *str, size_t len, uint8_t *const out, size_t *const out_len)
{
    size_t pack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t max_out_len = *out_len;

    if (len < ((max_out_len * 7) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        const uint8_t byte = (str[i] & 0x7F);
        const uint8_t mask  = ~(0xFFU << carryBitsCount);

        uint8_t toPush = 0;

        carryBits = (carryBits & mask) | (byte << carryBitsCount);

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
        }

        if (toPush <= 7) {
            if (toPush > 0) {
                const uint8_t lsh_bits = (7 - toPush);
                const uint8_t mask     = (0xFFU >> lsh_bits);

                carryBits      = (byte >> lsh_bits) & mask;
                carryBitsCount = toPush;
            }
        } else {
            assert(0);
        }
    }

    if (carryBitsCount > 0) {
        out[pack_len] = carryBits;
        ++pack_len;
    }

    *out_len = pack_len;

    return true;
}

bool unpack7Bit(const uint8_t *str7bit, size_t len, char *out, size_t *out_len)
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
                carryBitsCount = 2; // ???
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
            unpack_len += 2;
        }
        else {
            out[unpack_len] = carryBits & 0x7F;
            ++unpack_len;
        }
    }

    if (out[unpack_len-1]) {
        out[unpack_len] = '\0';
        ++unpack_len;
    }

    *out_len = unpack_len;

    return true;
}
