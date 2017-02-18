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
        out[pack_len] = carryBits;
        ++pack_len;
    }

    *out_len = pack_len;

    return true;
}

bool unpack7bit(const uint8_t *str7bit, size_t len, char *out, size_t *out_len)
{
    size_t unpack_len = 0;
    uint8_t carryBitsCount = 0;
    uint8_t carryBits = 0;

    size_t max_out_len = *out_len;

    if (max_out_len < ((len * 8)/ 7))
        return false;

    for (size_t i = 0; i < len; ++i) {
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

        carryBits >>= 7;
        carryBitsCount -= 7;

        if (toPush > 0) {
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

int main(int argc, char *argv[])
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
    return 0;
}
