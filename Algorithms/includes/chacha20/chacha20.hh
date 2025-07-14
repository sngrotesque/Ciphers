/**
 * @brief ChaCha20 encryption algorithm implemented in C++.
 *
 * This library provides a standalone implementation of the ChaCha20 stream cipher
 * as specified in RFC 8439. It faithfully adheres to the algorithmic requirements
 * defined in the standard, including 20-round quarter block operations and 32-bit
 * little-endian state structure.
 *
 * Note: This implementation includes **only the encryption primitive**.
 * It does **not** include the authentication mechanism (Poly1305),
 * and therefore does **not provide message integrity or authenticity guarantees**.
 *
 * Intended for educational use or integration into larger cryptographic systems
 * where message authentication is handled separately.
 */
#pragma once
#include "Common.hh"

typedef uint32_t u32;
typedef uint8_t u8;

class ChaCha20 {
private:
    alignas(16) u32 state[16]{0};

public:
    ChaCha20() = default;
    ChaCha20(const u8 k[32], const u8 n[12], u32 counter = 0);

    void crypto_stream(u8 *c, const u8 *m, size_t mlen);
};
