#pragma once
#include "Common.hh"

#define OP4_NATIVE_LE

constexpr u32 OP4_BL = 24; // block length
constexpr u32 OP4_KL = 32; // key length
constexpr u32 OP4_NK = 6;  // key word length
constexpr u32 OP4_NR = 8;  // Number of rounds
constexpr u32 OP4_NB = 4;  // Number of columns
constexpr u32 OP4_RKL = OP4_BL * OP4_NR; // Length of the round key

class OP4 {
private:
    u8 round_key[OP4_RKL]{0};

public:
    OP4() = default;
    OP4(const u8 k[OP4_KL]);

    void ecb_encrypt(u8 *ciphertext, const u8 *plaintext, size_t length);
    void ecb_decrypt(u8 *plaintext, const u8 *ciphertext, size_t length);

public:
    const u8 *const get_rk() const noexcept
    {
        return this->round_key;
    }
};
