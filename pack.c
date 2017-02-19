#include <assert.h>

#include "pack.h"

bool packBits(const uint8_t *str, const size_t len, uint8_t *const out, size_t *const pOutLen, uint8_t valueBits)
{
    size_t packLen = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t maxOutLen = *pOutLen;

    const uint8_t valueMask = (0x01U << valueBits) - 1U;
    assert(valueBits > 0 && valueBits <= 8);

    if (valueBits == 0 || valueBits > 8)
        return false;

    if (maxOutLen < ((len * valueBits) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        const uint8_t byte = (str[i] & valueMask);
        const uint8_t mask  = (0x01U << carryBitsCount) - 1U;

        uint8_t remainingBitsCount = 0;

        carryBits = (carryBits & mask) | (byte << carryBitsCount);

        if (carryBitsCount == 0) {
            remainingBitsCount = 0;
            carryBitsCount = valueBits;
        } else if (carryBitsCount <= 8) {
            remainingBitsCount = carryBitsCount + valueBits - 8;
            carryBitsCount = 0;

            out[packLen++] = carryBits;

            if (remainingBitsCount > 0) {
                const uint8_t lshBits = (valueBits - remainingBitsCount);

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

bool unpackBits(const uint8_t *packedStr, size_t len, char *out, size_t *pOutLen, uint8_t valueBits)
{
    size_t  unpackLen      = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits      = 0;
    uint8_t byte           = 0;
    const uint8_t valueMask = ((1U << valueBits) - 1U);
    size_t maxOutLen = *pOutLen;

    assert(valueBits > 0 && valueBits <= 8);

    if (valueBits == 0 || valueBits > 8)
        return false;

    if (maxOutLen < ((len * 8) / valueBits))
        return false;

    size_t i = 0;

    while (i < len) {
        uint8_t remainingBitsCount = 0;

        if (carryBitsCount < valueBits) {
            const uint8_t mask = 0xFFU >> (8 - carryBitsCount);

            byte = packedStr[i++];

            carryBits &= mask;
            carryBits |= byte << carryBitsCount;
            remainingBitsCount = carryBitsCount;
            carryBitsCount = 8;
        } else {
            remainingBitsCount = 0;
        }
        out[unpackLen++] = (carryBits & valueMask);

        carryBits     >>= valueBits;
        carryBitsCount -= valueBits;

        if (remainingBitsCount == 0) {
        } else if (remainingBitsCount < valueBits) {
            const uint8_t mask = 0xFFU >> (8 - carryBitsCount);
            carryBits &= mask;
            carryBits |= (byte >> (8 - remainingBitsCount)) << carryBitsCount;
            carryBitsCount += remainingBitsCount;
        } else {
            assert(0);
        }
    }

    if (carryBitsCount <= valueBits) {
        out[unpackLen++] = carryBits & valueMask;
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
