#ifndef PACK_H_
#define PACK_H_

#include <stdbool.h>
#include <stdint.h>

bool packBits(const uint8_t *str, const size_t len, uint8_t *const out, size_t *const out_len, uint8_t valueBits);

bool unpackBits(const uint8_t *packedStr, size_t len, char *out, size_t *out_len, uint8_t valueBits);

#endif /* PACK_H_ */
