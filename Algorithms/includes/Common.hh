#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define SIMD_SUPPORT
#ifdef SIMD_SUPPORT
#   include <immintrin.h>
#endif

#ifdef INCLUDE_CSTDBOOL
#   include <cstdbool>
#endif

#define NATIVE_LE

typedef uint32_t u32;
typedef uint8_t u8;

static inline u32 rotl32(const u32 &x, const u32 &n)
{
    return (x << n) | (x >> (32 - n));
}

static inline u32 rotr32(const u32 &x, const u32 &n)
{
    return (x >> n) | (x << (32 - n));
}

static inline u8 rotl8(const u32 &x, const u32 &n)
{
    return (x << n) | (x >> (8 - n));
}

static inline u8 rotr8(const u32 &x, const u32 &n)
{
    return (x >> n) | (x << (8 - n));
}

// LOAD or PACK - Little endianness
static inline u32 load32le(const u8 src[4])
{
    u32 w;
#   ifdef NATIVE_LE
    memcpy(&w, src, sizeof w);
#   else
    w  = ((u32)src[0]);
    w |= ((u32)src[1] <<  8);
    w |= ((u32)src[2] << 16);
    w |= ((u32)src[3] << 24);
#   endif
    return w;
}

static inline void pack32le(u8 dst[4], u32 w)
{
#   ifdef NATIVE_LE
    memcpy(dst, &w, sizeof w);
#   else
    dst[0] = (u8) w; w >>= 8;
    dst[1] = (u8) w; w >>= 8;
    dst[2] = (u8) w; w >>= 8;
    dst[3] = (u8) w;
#   endif
}

// LOAD or PACK - Big endianness
static inline u32 load32be(const u8 dst[4])
{
    return static_cast<u32>(dst[0]) << 24 |
           static_cast<u32>(dst[1]) << 16 |
           static_cast<u32>(dst[2]) <<  8 |
           static_cast<u32>(dst[3]);
}

static inline u32 load32be(const u8 &a, const u8 &b, const u8 &c, const u8 &d)
{
    return static_cast<u32>(a) << 24 |
           static_cast<u32>(b) << 16 |
           static_cast<u32>(c) <<  8 |
           static_cast<u32>(d);
}

static inline void pack32be(u8 dst[4], const u32 &w)
{
    dst[0] = w >> 24;
    dst[1] = w >> 16;
    dst[2] = w >> 8;
    dst[3] = w;
}

static inline void pack32be(u8 &a, u8 &b, u8 &c, u8 &d, const u32 &w)
{
    a = w >> 24;
    b = w >> 16;
    c = w >> 8;
    d = w;
}
