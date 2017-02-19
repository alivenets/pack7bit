#include <assert.h>
#
#include "pack7.h"

bool pack7Bit(const char *str, size_t len, uint8_t *const out, size_t *const pOutLen)
{
    size_t packLen = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t maxOutLen = *pOutLen;

    if (len < ((maxOutLen * 7) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        const uint8_t byte = (str[i] & 0x7F);
        const uint8_t mask  = ~(0xFFU << carryBitsCount);

        uint8_t remainingBitsCount = 0;

        carryBits = (carryBits & mask) | (byte << carryBitsCount);

        if (carryBitsCount == 0) {
            remainingBitsCount = 0;
            carryBitsCount = 7;
        } else if (carryBitsCount <= 8) {
            remainingBitsCount = carryBitsCount - 1;
            carryBitsCount = 0;

            out[packLen] = carryBits;
            ++packLen;

            if (remainingBitsCount > 0) {
                const uint8_t lshBits = (8 - 1 - remainingBitsCount);

                carryBits      = (byte >> lshBits);
                carryBitsCount = remainingBitsCount;
            }
        } else {
            assert(0);
        }
    }

    if (carryBitsCount > 0) {
        out[packLen] = carryBits;
        ++packLen;
    }

    *pOutLen = packLen;

    return true;
}

bool unpack7Bit(const uint8_t *str7bit, size_t len, char *out, size_t *pOutLen)
{
    size_t  unpackLen = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;

    size_t maxOutLen = *pOutLen;

    if (maxOutLen < ((len * 8)/ 7))
        return false;

    for (size_t i = 0; i < len; ++i) {
        const uint8_t byte = str7bit[i];

        uint8_t remainingBitsCount = 0;

        const uint8_t mask  = 0xFFU >> (8 - carryBitsCount);
        const uint8_t mask2 = 0xFFU >> carryBitsCount;

        if (carryBitsCount <= 8) {
            carryBits &= mask;
            carryBits |= (byte & mask2) << carryBitsCount;
            remainingBitsCount = carryBitsCount;
            carryBitsCount = 8;
        } else {
            assert(0);
        }

        out[unpackLen] = (carryBits & 0x7F);
        ++unpackLen;

        carryBits >>= 7;
        carryBitsCount -= 7;

        if (remainingBitsCount <= 8) {
            carryBits |= (byte >> (8 - remainingBitsCount)) << 1;
            if (remainingBitsCount == 8) {
                out[unpackLen] = (carryBits & 0x7F);
                ++unpackLen;
                carryBits = carryBits >> 7;
                carryBits |= (byte >> 7) << 1;
                carryBitsCount = 2; // ???
            } else {
                carryBitsCount += remainingBitsCount;
            }
        } else {
            assert(0);
        }
    }

    if (carryBitsCount > 0) {
        if (carryBitsCount == 8) {
            out[unpackLen] = (carryBits & 0x7F);
            out[unpackLen+1] = (carryBits >> 7) & 0x01;
            unpackLen += 2;
        }
        else {
            out[unpackLen] = carryBits & 0x7F;
            ++unpackLen;
        }
    }

    if (out[unpackLen-1]) {
        out[unpackLen] = '\0';
        ++unpackLen;
    }

    *pOutLen = unpackLen;

    return true;
}
