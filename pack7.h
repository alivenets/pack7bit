#include <stdbool.h>
#include <stdint.h>

bool pack7Bit(const char *str, size_t len, uint8_t *const out, size_t *const out_len);

bool unpack7Bit(const uint8_t *str7bit, size_t len, char *out, size_t *out_len);
