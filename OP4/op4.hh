#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>

#if defined(_MSC_VER) || __cplusplus < 201703
#   include <cstdbool>
#endif

typedef uint32_t u32;
typedef uint8_t u8;

#define OP4_NATIVE_LE // little-endian

constexpr u32 OP4_BL = 16; // block length
constexpr u32 OP4_KL = 32; // key length
constexpr u32 OP4_NL = 12; // Nonce length
constexpr u32 OP4_NK = 4;  // key word length
constexpr u32 OP4_NR = 8;  // Number of rounds
constexpr u32 OP4_NB = 4;  // Number of columns
constexpr u32 OP4_RKL = OP4_BL * OP4_NR; // Length of the round key

class OP4 {
private:
    u8 round_key[OP4_RKL]{0};

public:
    OP4() = default;
    OP4(const u8 k[OP4_KL]);

    void ecb_encrypt(u8 *out, const u8 *in, size_t length);
    void ecb_decrypt(u8 *out, const u8 *in, size_t length);

    void cbc_encrypt(u8 *out, const u8 *in,
                    size_t length, const u8 iv[OP4_BL]);
    void cbc_decrypt(u8 *out, const u8 *in,
                    size_t length, const u8 iv[OP4_BL]);

    void ofb_xcrypt(u8 *out, const u8 *in,
                    size_t length, const u8 iv[OP4_NL]);

    void ctr_xcrypt(u8 *out, const u8 *in, size_t length,
            const u8 nonce[OP4_NL], size_t counter = 0);

public:
    const u8 *const get_rk() const noexcept
    {
        return this->round_key;
    }
};
