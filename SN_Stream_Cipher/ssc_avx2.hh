#pragma once
#include <immintrin.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>

typedef uint32_t u32;
typedef uint8_t  u8;

constexpr u32 WukSSC_KSLEN    = 64; // keystream length
constexpr u32 WukSSC_KEYLEN   = 32; // key length
constexpr u32 WukSSC_NONCELEN = 12; // nonce length
constexpr u32 WukSSC_STATELEN = (WukSSC_KSLEN / sizeof(u32)); // state length

#ifndef WukSSC_MIN
#   define WukSSC_MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

class WukSSC {
private:
    alignas(32) u32 state[WukSSC_STATELEN]{0};

public:
    WukSSC() = default;
    WukSSC(const u8 key[WukSSC_KEYLEN],
           const u8 nonce[WukSSC_NONCELEN],
           const u32 counter = 0);

public:
    void init(const u8 key[WukSSC_KEYLEN],
              const u8 nonce[WukSSC_NONCELEN],
              const u32 counter = 0);
    void xcrypt(u8 *ciphertext, const u8 *plaintext, size_t length);

    const u8 *get_state() const noexcept
    {
        return reinterpret_cast<const u8 *>(this->state);
    }
};
