#pragma once
#include "Common.hh"

constexpr u32 OP4_BL = 16; // block length
constexpr u32 OP4_KL = 32; // key length
constexpr u32 OP4_NL = 12; // Nonce length
constexpr u32 OP4_NK = 4;  // key word length
constexpr u32 OP4_NR = 8;  // Number of rounds
constexpr u32 OP4_RKL = OP4_BL * OP4_NR; // Length of the round key

class OP4 {
private:
    alignas(16) u8 round_key[OP4_RKL]{0};
    u32 counter = 0U;

public:
    OP4() = default;
    OP4(const u8 k[OP4_KL]);

    void ecb_encrypt(u8 *out, const u8 *in, size_t length);
    void ecb_decrypt(u8 *out, const u8 *in, size_t length);

    void cbc_encrypt(u8 *out, const u8 *in, size_t length,
               const u8 iv[OP4_BL]);
    void cbc_decrypt(u8 *out, const u8 *in, size_t length,
               const u8 iv[OP4_BL]);

    void ofb_stream(u8 *out, const u8 *in, size_t length,
              const u8 iv[OP4_BL]);

    void ctr_stream(u8 *out, const u8 *in, size_t length,
              const u8 nonce[OP4_NL]);

public:
    void set_counter(u32 counter) noexcept
    {
        this->counter = counter;
    }

    const u8 *get_rk() const noexcept
    {
        return this->round_key;
    }
};
