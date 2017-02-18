#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

size_t pack7bit(const char *str, uint8_t *out, size_t out_len)
{
    const size_t len = strlen(str);
    size_t pack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;

    for(size_t i = 0; i < len; ++i) {
        uint8_t byte = (str[i] & 0x7F);

        uint8_t toPush = 0;

        switch (carryBitsCount) {
            case 0:
                carryBits &= 0x00;
                carryBits |= (byte & 0xFF) << 0;
                carryBitsCount = 7;
                toPush = 0;
                break;

            case 1:
                carryBits &= 0x01;
                carryBits |= (byte & 0x7F) << 1;
                carryBitsCount = 8;
                toPush = 0;
                break;

            case 2:
                carryBits &= 0x03;
                carryBits |= (byte & 0x3F) << 2;
                carryBitsCount = 8;
                toPush = 1;
                break;

            case 3:
                carryBits &= 0x07;
                carryBits |= (byte & 0x1F) << 3;
                carryBitsCount = 8;
                toPush = 2;
                break;

            case 4:
                carryBits &= 0x0F;
                carryBits |= (byte & 0x0F) << 4;
                carryBitsCount = 8;
                toPush = 3;
                break;

            case 5:
                carryBits &= 0x1F;
                carryBits |= (byte & 0x07) << 5;
                carryBitsCount = 8;
                toPush = 4;
                break;

            case 6:
                carryBits &= 0x3F;
                carryBits |= (byte & 0x03) << 6;
                carryBitsCount = 8;
                toPush = 5;
                break;

            case 7:
                carryBits &= 0x7F;
                carryBits |= (byte & 0x01) << 7;
                carryBitsCount = 8;
                toPush = 6;
                break;

            case 8:
                carryBitsCount = 8;
                toPush = 7;
                break;

            default:
                assert(0);
                break;
        }

        if (carryBitsCount == 8) {
            out[pack_len] = carryBits;
            ++pack_len;

            if (pack_len >= out_len)
                break;

            carryBitsCount = 0;

            if (toPush > 0) {
                //push remaining bytes to carryBits
                switch (toPush) {
                    case 0:
                        break;

                    case 1:
                        carryBits = byte >> 6;
                        carryBitsCount = 1;
                        break;

                    case 2:
                        carryBits = byte >> 5;
                        carryBitsCount = 2;
                        break;

                    case 3:
                        carryBits = byte >> 4;
                        carryBitsCount = 3;
                        break;

                    case 4:
                        carryBits = byte >> 3;
                        carryBitsCount = 4;
                        break;

                    case 5:
                        carryBits = byte >> 2;
                        carryBitsCount = 5;
                        break;

                    case 6:
                        carryBits = byte >> 1;
                        carryBitsCount = 6;
                        break;

                    case 7:
                        carryBits = byte >> 0;
                        carryBitsCount = 7;
                        break;

                    case 8:
                    default:
                        assert(0);
                        break;
                }
            }
        }
    }

    if (carryBitsCount > 0) {
        if (pack_len < out_len)
            out[pack_len] = carryBits;

        ++pack_len;
    }

    return pack_len;
}

size_t unpack7bit(const uint8_t *str7bit, size_t in_len, char *out, size_t out_len)
{
    size_t unpack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;

    for (size_t i = 0; i < in_len; ++i) {
        const uint8_t byte = str7bit[i];
        uint8_t toPush = 0;

        switch (carryBitsCount) {
            case 0:
                carryBits &= 0x00;
                carryBits |= (byte & 0xFF) << 0;
                carryBitsCount = 8;
                toPush = 0;
                break;
            case 1:
                carryBits &= 0x01;
                carryBits |= (byte & 0x7F) << 1;
                carryBitsCount = 8;
                toPush = 1;
                break;
            case 2:
                carryBits &= 0x03;
                carryBits |= (byte & 0x3F) << 2;
                carryBitsCount = 8;
                toPush = 2;
                break;
            case 3:
                carryBits &= 0x07;
                carryBits |= (byte & 0x1F) << 3;
                carryBitsCount = 8;
                toPush = 3;
                break;
            case 4:
                carryBits &= 0x0F;
                carryBits |= (byte & 0x0F) << 4;
                carryBitsCount = 8;
                toPush = 4;
                break;
            case 5:
                carryBits &= 0x1F;
                carryBits |= (byte & 0x07) << 5;
                carryBitsCount = 8;
                toPush = 5;
                break;
            case 6:
                carryBits &= 0x3F;
                carryBits |= (byte & 0x03) << 6;
                carryBitsCount = 8;
                toPush = 6;
                break;
            case 7:
                carryBits &= 0x7F;
                carryBits |= (byte & 0x01) << 7;
                carryBitsCount = 8;
                toPush = 7;
                break;
            case 8:
                carryBits &= 0xFF;
                carryBits |= (byte & 0x00) << 8;
                carryBitsCount = 8;
                toPush = 8;
                break;
            default:
                assert(0);
                carryBitsCount = 8;
                break;
        }

        out[unpack_len] = (carryBits & 0x7F);
        ++unpack_len;

        if (unpack_len >= out_len)
            break;

        carryBits >>= 7;
        carryBitsCount -= 7;

        if (toPush > 0) {
            if (unpack_len < out_len) {
                switch (toPush) {
                    case 0:
//                      carryBits = byte >> 0;
                        carryBitsCount += 0;
                        break;
                    case 1:
                        carryBits |= (byte >> 7) << 1;
                        carryBitsCount += 1;
                        break;
                    case 2:
                        carryBits |= (byte >> 6) << 1;
                        carryBitsCount += 2;
                        break;
                    case 3:
                        carryBits |= (byte >> 5) << 1;
                        carryBitsCount += 3;
                        break;
                    case 4:
                        carryBits |= (byte >> 4) << 1;
                        carryBitsCount += 4;
                        break;
                    case 5:
                        carryBits |= (byte >> 3) << 1;
                        carryBitsCount += 5;
                        break;
                    case 6:
                        carryBits |= (byte >> 2) << 1;
                        carryBitsCount += 6;
                        break;
                    case 7:
                        carryBits |= (byte >> 1) << 1;
                        carryBitsCount += 7;
                        break;
                    case 8:
                        carryBits |= (byte >> 0) << 1;
                        out[unpack_len] = (carryBits & 0x7F);
                        ++unpack_len;
                        carryBits = carryBits >> 7;
                        carryBits |= (byte >> 7) << 1;
                        carryBitsCount = 2;

                        break;
                    default:
                        assert(0);
                        break;

                }
            }
        }
    }

    if (carryBitsCount > 0) {
        if (carryBitsCount == 8) {
            if (unpack_len < out_len)
                out[unpack_len] = (carryBits & 0x7F);
            if (unpack_len+1 < out_len)
                out[unpack_len+1] = (carryBits >> 7) & 0x01;
        }
        else {
            if (unpack_len < out_len)
                out[unpack_len] = carryBits & 0x7F;
        }
    }

    return unpack_len;
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

int main(int argc, char *argv[])
{
    const char str[] = "~{}~{}[][]ABCDEFGH[][]~{}~{}abcdefgh~{}~{}1234567890~{}~{}";
    uint8_t buf[sizeof(str)] = {0};
    char unpack_buf[sizeof(str)] = "";
    printf("Input string: %s\n", str);

    printf("Input HEX:\n");
    dumphex((const uint8_t*)str, sizeof(str));

    size_t len;
    len = pack7bit(str, buf, sizeof(buf));

    printf("Pack7:\n");
    dumphex(buf, sizeof(buf));

    len = unpack7bit(buf, sizeof(buf), unpack_buf, sizeof(unpack_buf));

    printf("Unpack7 HEX:\n");
    dumphex(unpack_buf, sizeof(unpack_buf));

    printf("Unpack7: %s\n", unpack_buf);

    if(strcmp(str, unpack_buf) == 0) {
        printf("--OK--\n");
    }
    else {
        printf("!!! CMP_FAIL !!!\n");
    }
    return 0;
}
