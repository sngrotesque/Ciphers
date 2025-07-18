#pragma once
#include "Common.hh"

constexpr u32 OP4S_KSL = 64; // keystream (state) length
constexpr u32 OP4S_KL  = 32; // key length
constexpr u32 OP4S_NL  = 12; // nonce length

class OP4S {
private:
    alignas(16) u32 state[OP4S_KSL / sizeof(u32)];

public:
    OP4S() = default;
    OP4S(const u8 k[OP4S_KL], const u8 n[OP4S_NL], u32 counter = 0);

public:
    void crypto_stream(u8 *out, u8 *in, size_t length);
};
