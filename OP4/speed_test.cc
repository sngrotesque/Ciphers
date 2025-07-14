#include <chrono>

#include <cstdio>
#include <cstdint>
#include <cstring>

typedef uint32_t u32;
typedef uint8_t  u8;

#define SI(T) static inline T

SI(double) timer() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
}

void print_hex(const uint8_t *data, size_t len, size_t num, bool newline, bool indent)
{
    for(size_t i = 0; i < len; ++i) {
        if(indent && ((i) % num == 0)) {
            printf("\t");
        }

#       ifdef TEST_COLOR
        if (data[i] == 0) {
            printf("\x1b[91m""%02x""\x1b[0m", data[i]);
        } else if (data[i] == 255) {
            printf("\x1b[93m""%02x""\x1b[0m", data[i]);
        } else {
            printf("%02x", data[i]);
        }
#       else
        printf("%02x", data[i]);
#       endif

        printf(((i + 1) % num) ? " " : "\n");
    }
    if(newline) printf("\n");
}

#define LOAD32(dst) load32_le(dst)
SI(u32) load32_le(const u8 dst[4])
{
#   ifdef NATIVE_LE
    u32 w;
    memcpy(&w, dst, sizeof(w));
    return w;
#   else
    return  (u32)(dst[3] << 24) |
            (u32)(dst[2] << 16) |
            (u32)(dst[1] << 8) |
            (u32) dst[0];
#   endif
}

#define PACK32(dst, w) pack32_le(dst, w)
SI(void) pack32_le(u8 dst[4], const u32 &w)
{
#   ifdef NATIVE_LE
    memcpy(dst, &w, sizeof(w));
#   else
    dst[3] = w >> 24;
    dst[2] = w >> 16;
    dst[1] = w >> 8;
    dst[0] = w;
#   endif
}

SI(u32) ROTL32(const u32 &x, const u32 &n)
{
    return (x << n) | (x >> (32 - n));
}

SI(void) test_func_1(u8 block[16])
{
    u32 v1 = LOAD32(block + 4);
    u32 v2 = LOAD32(block + 8);
    u32 v3 = LOAD32(block + 12);

    v1 = ROTL32(v1, 8);
    v2 = ROTL32(v2, 16);
    v3 = ROTL32(v3, 24);

    PACK32(block + 4,  v1);
    PACK32(block + 8,  v2);
    PACK32(block + 12, v3);
}

SI(void) test_func_2(u8 block[16])
{
    u8 swap   = block[4];
    block[4]  = block[5];
    block[5]  = block[6];
    block[6]  = block[7];
    block[7]  = swap;

    swap      = block[8];
    block[8]  = block[10];
    block[10] = swap;
    swap      = block[9];
    block[9]  = block[11];
    block[11] = swap;

    swap      = block[15];
    block[15] = block[14];
    block[14] = block[13];
    block[13] = block[12];
    block[12] = swap;
}

void speed_test(void (*func)(u8 *), u8 *block, size_t count = 67108864, u32 rounds = 5)
{
    double total_time = 0;
    double average_time = 0;
    double start, stop, current_time;

    for (size_t r = 0; r < rounds; ++r) {
        start = timer();
        for (size_t c = 0; c < count; ++c) {
            func(block);
        }
        stop = timer();
        current_time = stop - start;
        printf("Taken time:         %.4lf.\n", current_time);
        total_time += current_time;
    }
    average_time = total_time / rounds;
    printf("Total taken time:   %.4lf.\n", total_time);
    printf("Average taken time: %.4lf.\n", average_time);
}

int main()
{
    u8 state[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };

#   if defined(TEST_FUNC_1)
    speed_test(test_func_1, state);
#   elif defined(TEST_FUNC_2)
    speed_test(test_func_2, state);
#   endif

    print_hex(state, 16, 4, true, true);

    return 0;
}
