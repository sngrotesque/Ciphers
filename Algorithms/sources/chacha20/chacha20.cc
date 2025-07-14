#include "chacha20/chacha20.hh"

#define U32C(x) x##U

#define QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = rotl32(d, 16); \
    c += d; b ^= c; b = rotl32(b, 12); \
    a += b; d ^= a; d = rotl32(d, 8);  \
    c += d; b ^= c; b = rotl32(b, 7);

static inline u32 load32_le(const u8 src[4])
{
    u32 w = (u32) src[0];
    w |= (u32) src[1] <<  8;
    w |= (u32) src[2] << 16;
    w |= (u32) src[3] << 24;
    return w;
}

static inline void store32_le(u8 dst[4], u32 w)
{
    dst[0] = (u8) w; w >>= 8;
    dst[1] = (u8) w; w >>= 8;
    dst[2] = (u8) w; w >>= 8;
    dst[3] = (u8) w;
}

static inline u32 rotl32(u32 x, u32 n)
{
    return (x << n) | (x >> (32 - n));
}

static inline void keystream_add_state(u32 ks[16], const u32 state[16])
{
    ks[0]  += state[0];  ks[1]  += state[1];
    ks[2]  += state[2];  ks[3]  += state[3];
    ks[4]  += state[4];  ks[5]  += state[5];
    ks[6]  += state[6];  ks[7]  += state[7];
    ks[8]  += state[8];  ks[9]  += state[9];
    ks[10] += state[10]; ks[11] += state[11];
    ks[12] += state[12]; ks[13] += state[13];
    ks[14] += state[14]; ks[15] += state[15];
}

ChaCha20::ChaCha20(const u8 k[32], const u8 n[12], u32 counter)
{
    u8 b[4]{0};
    store32_le(b, counter);

    // constants
    this->state[0]  = U32C(0x61707865);
    this->state[1]  = U32C(0x3320646e);
    this->state[2]  = U32C(0x79622d32);
    this->state[3]  = U32C(0x6b206574);

    // key
    this->state[4]  = load32_le(k + 0);
    this->state[5]  = load32_le(k + 4);
    this->state[6]  = load32_le(k + 8);
    this->state[7]  = load32_le(k + 12);
    this->state[8]  = load32_le(k + 16);
    this->state[9]  = load32_le(k + 20);
    this->state[10] = load32_le(k + 24);
    this->state[11] = load32_le(k + 28);

    // counter & nonce
    this->state[12] = load32_le(b);
    this->state[13] = load32_le(n + 0);
    this->state[14] = load32_le(n + 4);
    this->state[15] = load32_le(n + 8);
}

void ChaCha20::crypto_stream(u8 *c, const u8 *m, size_t mlen)
{
    u32 tmp[16]{0};
    u8 *keystream = reinterpret_cast<u8 *>(tmp);

    for (size_t i = 0, ki = 64; i < mlen; ++i, ++ki) {
        if (ki == 64) {
            memcpy(tmp, this->state, 64);

            for (u32 r = 0; r < 10; ++r) {
                QUARTERROUND(tmp[0], tmp[4], tmp[8],  tmp[12]);
                QUARTERROUND(tmp[1], tmp[5], tmp[9],  tmp[13]);
                QUARTERROUND(tmp[2], tmp[6], tmp[10], tmp[14]);
                QUARTERROUND(tmp[3], tmp[7], tmp[11], tmp[15]);
                QUARTERROUND(tmp[0], tmp[5], tmp[10], tmp[15]);
                QUARTERROUND(tmp[1], tmp[6], tmp[11], tmp[12]);
                QUARTERROUND(tmp[2], tmp[7], tmp[8],  tmp[13]);
                QUARTERROUND(tmp[3], tmp[4], tmp[9],  tmp[14]);
            }
            keystream_add_state(tmp, this->state);

            this->state[12]++;

            ki = 0;
        }

        c[i] = m[i] ^ keystream[ki];
    }
}
