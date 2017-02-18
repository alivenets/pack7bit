#include <assert.h>

#include "pack.h"

bool packBits(const uint8_t *str, const size_t len, uint8_t *const out, size_t *const out_len, uint8_t valueBits)
{
    size_t pack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const size_t max_out_len = *out_len;

    const uint8_t valueMask = ((1U << valueBits) - 1U);

    assert(valueBits > 0 && valueBits <= 8);

    if (valueBits == 0 || valueBits > 8)
        return false;

    if (len < ((max_out_len * valueBits) / 8))
        return false;

    for(size_t i = 0; i < len; ++i) {
        const uint8_t byte = (str[i] & valueMask);
        const uint8_t mask  = ~(0xFFU << carryBitsCount);

        uint8_t toPush = 0;

        carryBits = (carryBits & mask) | (byte << carryBitsCount);

        if (carryBitsCount == 0) {
            toPush = 0;
            carryBitsCount = valueBits;
        } else if (carryBitsCount <= 8) {
            toPush = carryBitsCount - (8 - valueBits);
            carryBitsCount = 8;
        } else {
            assert(0);
        }

        if (carryBitsCount == 8) {
            out[pack_len] = carryBits;
            ++pack_len;
            carryBitsCount = 0;
        }

        if (toPush <= valueBits) {
            if (toPush > 0) {
                const uint8_t lsh_bits = (valueBits - toPush);
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

bool unpackBits(const uint8_t *packedStr, size_t len, uint8_t *out, size_t *out_len, uint8_t valueBits)
{
    size_t  unpack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;
    const uint8_t valueMask = ((1U << valueBits) - 1U);
    size_t max_out_len = *out_len;

    assert(valueBits > 0 && valueBits <= 8);

    if (valueBits == 0 || valueBits > 8)
        return false;

    if (max_out_len < ((len * 8)/ valueBits))
        return false;

    for (size_t i = 0; i < len; ++i) {
        const uint8_t byte = packedStr[i];

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

        out[unpack_len] = (carryBits & valueMask);
        ++unpack_len;

        carryBits >>= valueBits;
        carryBitsCount -= valueBits;

        if (toPush <= 8) {
            carryBits |= (byte >> (8 - toPush)) << (8 - valueBits);
            if (toPush == 8) {
                out[unpack_len] = carryBits & valueMask;
                ++unpack_len;
                carryBits = carryBits >> valueBits;
                carryBits |= (byte >> valueBits) << (8 - valueBits);
                carryBitsCount = ((8 - valueBits) * 2); // ???
            } else {
                carryBitsCount += toPush;
            }
        } else {
            assert(0);
        }
    }

    if (carryBitsCount > 0) {
        if (carryBitsCount == 8) {
            out[unpack_len] = carryBits & valueMask;
            out[unpack_len+1] = (carryBits >> valueBits) & (0xFFU >> valueBits);
            unpack_len += 2;
        }
        else {
            out[unpack_len] = (carryBits & valueMask);
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
