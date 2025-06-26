#pragma once
#include <immintrin.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#   include <Windows.h>
#endif

typedef uint32_t u32;
typedef uint8_t  u8;

static inline void memory_zero(void *buffer, size_t length)
{
    memset(buffer, 0x00, length);
}

static inline void memory_secure(void *buffer, size_t length)
{
#   if defined(WUK_PLATFORM_WINOS)
    SecureZeroMemory(buffer, length);
#   elif defined(WUK_PLATFORM_LINUX)
#   ifdef WUK_PLATFORM_ANDROID
    volatile char *ptr = (volatile char *)p;
    do {*ptr++ = 0; } while (--length);
#   else
    explicit_bzero(buffer, length);
#   endif
#   endif
}

#if (__cplusplus >= 201703L) || (defined(_MSC_VER) && (_MSVC_LANG >= 201703))
#   include <new>
#   define SSC_CPP17_SUPPORT true
#else
#   include <malloc.h>
#   define SSC_CPP17_SUPPORT false
#endif

template <typename T>
static inline void ssc_alloc(size_t length, uint32_t aligned_length = 32U)
{
#   if SSC_CPP17_SUPPORT
    return new (std::alig_val_t(aligned_length), std::nothrow) type[memory_size];
#   else
    return static_cast<T>(malloc(length));
#   endif
}

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
