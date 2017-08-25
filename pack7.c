#include <assert.h>

#include "pack7.h"

bool pack7Bit(const char *str, size_t len, uint8_t *const out, size_t *const pOutLen)
{
    size_t packLen = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t maxOutLen = *pOutLen;

    if (maxOutLen < ((len * 7) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        const uint8_t byte = (str[i] & 0x7F);
        const uint8_t mask  = (0x01U << carryBitsCount) - 1;

        uint8_t remainingBitsCount = 0;

        carryBits = (carryBits & mask) | (byte << carryBitsCount);

        if (carryBitsCount == 0) {
            remainingBitsCount = 0;
            carryBitsCount = 7;
        } else if (carryBitsCount <= 8) {
            remainingBitsCount = carryBitsCount + 7 - 8;
            carryBitsCount = 0;

            out[packLen++] = carryBits;

            if (remainingBitsCount > 0) {
                const uint8_t lshBits = (8 - 1 - remainingBitsCount);

                carryBits      = (byte >> lshBits);
                carryBitsCount = remainingBitsCount;
            }
        } else {
            assert(0);
            break;
        }
    }

    if (carryBitsCount > 0) {
        out[packLen++] = carryBits;
    }

    *pOutLen = packLen;

    return true;
}

bool unpack7Bit(const uint8_t *str7bit, size_t len, char *out, size_t *pOutLen)
{
    size_t  unpackLen      = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits      = 0;
    uint8_t byte           = 0;

    size_t maxOutLen = *pOutLen;

    if (maxOutLen < ((len * 8) / 7))
        return false;

    size_t i = 0;
    while (i < len) {
        uint8_t remainingBitsCount = 0;

        if (carryBitsCount < (8 - 1)) {
            const uint8_t mask = 0xFFU >> (8 - carryBitsCount);

            byte = str7bit[i++];

            carryBits &= mask;
            carryBits |= byte << carryBitsCount;
            remainingBitsCount = carryBitsCount;
            carryBitsCount = 8;
        } else {
            remainingBitsCount = 0;
        }

        out[unpackLen++] = (carryBits & 0x7F);

        carryBits     >>= (8 - 1);
        carryBitsCount -= (8 - 1);

        if (remainingBitsCount == 0) {
        } else if (remainingBitsCount < (8 - 1)) {
            const uint8_t mask = 0xFFU >> (8 - carryBitsCount);
            carryBits &= mask;
            carryBits |= (byte >> (8 - remainingBitsCount)) << carryBitsCount;
            carryBitsCount += remainingBitsCount;
        } else {
            assert(0);
        }
    }

    if (carryBitsCount <= (8 - 1)) {
        out[unpackLen++] = carryBits & 0x7F;
    } else {
        assert(0);
        return false;
    }

    if (out[unpackLen-1])
        out[unpackLen++] = '\0';

    *pOutLen = unpackLen;

    assert(unpackLen <= maxOutLen);

    return true;
}
