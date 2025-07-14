#include "op4.hh"
#include "misc.hh"
#include <iostream>

static inline const u8 MUL_COEFFS[OP4_BL] = {
    113, 233, 97,  211,
    71,  223, 241, 53,
    23,  47,  79,  37,
    73,  193, 227, 191
};

static inline u32 ROTL32(const u32 &x, const u32 &n)
{
    return (x << n) | (x >> (32 - n));
}

static inline u32 LOAD32_LE(u8 dst[4])
{
#   ifdef OP4_NATIVE_LE
    u32 w;
    memcpy(&w, dst, sizeof(w));
    return w;
#   else
    return (static_cast<u32>(dst[0]) <<  0) |
           (static_cast<u32>(dst[1]) <<  8) |
           (static_cast<u32>(dst[2]) << 16) |
           (static_cast<u32>(dst[3]) << 24);
#   endif
}

static inline void PACK32_LE(u8 dst[4], const u32 &w)
{
#   ifdef OP4_NATIVE_LE
    memcpy(dst, &w, sizeof(w));
#   else
    dst[0] = static_cast<u8>(w >> 0);
    dst[1] = static_cast<u8>(w >> 8);
    dst[2] = static_cast<u8>(w >> 16);
    dst[3] = static_cast<u8>(w >> 24);
#   endif
}

static inline void shift_bits_add(u8 state[OP4_BL])
{
    u32 v0, v1, v2, v3;

    v0 = LOAD32_LE(state + 0);
    v1 = LOAD32_LE(state + 4);
    v2 = LOAD32_LE(state + 8);
    v3 = LOAD32_LE(state + 12);

    v0 = ROTL32(v0, 13) + v1 + v2 + v3;
    v1 = ROTL32(v1, 19) + v2 + v3 + v0;
    v2 = ROTL32(v2, 11) + v3 + v0 + v1;
    v3 = ROTL32(v3, 17) + v0 + v1 + v2;

    PACK32_LE(state,      v0);
    PACK32_LE(state + 4,  v1);
    PACK32_LE(state + 8,  v2);
    PACK32_LE(state + 12, v3);
}

static inline void multiply(u8 state[OP4_BL])
{
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] *= MUL_COEFFS[i] & 0xff;
    }
}

int main()
{
    u8 block[OP4_BL] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };
    u8 state[OP4_BL] = {0};

    // Only 3 rounds are needed, and there is
    // no key involved in the operation.
    constexpr u32 rounds = 3;

    memcpy(state, block, OP4_BL);

    std::cout << "Plaintext:\n";
    print_hex(state, OP4_BL, OP4_BL, true, true);

    for (u32 r = 0; r < rounds; ++r) {
        shift_bits_add(state);
        multiply(state);
    }

    std::cout << "Cipherext:\n";
    print_hex(state, OP4_BL, OP4_BL, true, true);

    ///////////////////////////////////////////

    memcpy(state, block, OP4_BL);
    state[0] ^= 1; // Modify one bit.

    std::cout << "Plaintext:\n";
    print_hex(state, OP4_BL, OP4_BL, true, true);

    for (u32 r = 0; r < rounds; ++r) {
        shift_bits_add(state);
        multiply(state);
    }

    std::cout << "Cipherext:\n";
    print_hex(state, OP4_BL, OP4_BL, true, true);

    return 0;
}

