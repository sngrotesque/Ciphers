#include "ssc_avx2.hh"

#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define U32C(x) x##U

static inline u32 LOAD32_LE(const u8 d[4])
{
    u32 w  = static_cast<u32>(d[0]);
        w |= static_cast<u32>(d[1]) <<  8;
        w |= static_cast<u32>(d[2]) << 16;
        w |= static_cast<u32>(d[3]) << 24;
    return w;
}

static inline void PACK32_LE(u8 d[4], u32 &w)
{
    d[0] = static_cast<u8>(w); w >>= 8;
    d[0] = static_cast<u8>(w); w >>= 8;
    d[0] = static_cast<u8>(w); w >>= 8;
    d[0] = static_cast<u8>(w);
}

static inline void rotl128_add()
{
    
}
