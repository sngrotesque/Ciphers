#pragma once
#include "Common.hh"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint8_t  u8;

constexpr u32 WukSSC_KSLEN    = 64;
constexpr u32 WukSSC_KEYLEN   = 32;
constexpr u32 WukSSC_NONCELEN = 16;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

class WukSSC {
private:
    alignas(16) u32 state[16]{0};

public:
    WukSSC() = default;
    WukSSC(const u8 key[WukSSC_KEYLEN], const u8 nonce[WukSSC_NONCELEN], u32 counter = 0);

public:
    void init(const u8 key[WukSSC_KEYLEN], const u8 nonce[WukSSC_NONCELEN], u32 counter = 0);
    void xcrypt(u8 *ciphertext, const u8 *plaintext, size_t length);

    const u8 *get_state() const noexcept
    {
        return reinterpret_cast<const u8 *>(state);
    }
};
