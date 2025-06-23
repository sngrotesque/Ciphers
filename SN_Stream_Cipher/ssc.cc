#include "ssc.hh"

#ifdef _MSC_VER
#   define ROTL32(x, n) _rotl(x, n)
#else
#   define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#endif

#define U32C(x) x##U

static inline uint32_t load32_le(const uint8_t dst[4])
{
    uint32_t w = 0;
#   if WUK_IS_LITTLE_ENDIAN
    memcpy(&w, dst, sizeof w);
#   else
    w =  (uint32_t) dst[0];
    w |= (uint32_t) dst[1] <<  8;
    w |= (uint32_t) dst[2] << 16;
    w |= (uint32_t) dst[3] << 24;
#   endif
    return w;
}

static inline void store32_le(uint8_t dst[4], uint32_t w)
{
#   if WUK_IS_LITTLE_ENDIAN
    memcpy(dst, &w, sizeof w);
#   else
    dst[0] = (uint8_t) w; w >>= 8;
    dst[1] = (uint8_t) w; w >>= 8;
    dst[2] = (uint8_t) w; w >>= 8;
    dst[3] = (uint8_t) w;
#   endif
}

static inline void mix(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d)
{
    a ^= ROTL32(b + a, 7);
    b ^= ROTL32(c ^ b, 13);
    c ^= ROTL32(d + c, 17);
    d ^= ROTL32(a ^ d, 3);
}

static inline void keystream_mixing(uint32_t state[16])
{
    // Oblique mixing
    mix(state[0],  state[5],  state[10], state[15]);
    mix(state[4],  state[9],  state[14], state[3]);
    mix(state[8],  state[13], state[2],  state[7]);
    mix(state[12], state[1],  state[6],  state[11]);

    // Vertical mixing
    mix(state[0],  state[4],  state[8],  state[12]);
    mix(state[1],  state[5],  state[9],  state[13]);
    mix(state[2],  state[6], state[10],  state[14]);
    mix(state[3],  state[7], state[11],  state[15]);
}

WukSSC::WukSSC(const uint8_t key[WukSSC_KEYLEN], const uint8_t nonce[WukSSC_NONCELEN], uint32_t counter)
{
    this->init(key, nonce, counter);
}

void WukSSC::init(const uint8_t key[WukSSC_KEYLEN], const uint8_t nonce[WukSSC_NONCELEN], uint32_t counter)
{
    uint8_t count[4]{};
    store32_le(count, counter);

    this->state[0]  = load32_le(count);
    this->state[1]  = load32_le(nonce);
    this->state[2]  = load32_le(nonce + 4);
    this->state[3]  = load32_le(nonce + 8);

    this->state[4]  = U32C(0xf6078754);
    this->state[5]  = U32C(0x35024727);
    this->state[6]  = U32C(0x07963435);
    this->state[7]  = U32C(0xe2275686);

    this->state[8]  = load32_le(key);
    this->state[9]  = load32_le(key + 4);
    this->state[10] = load32_le(key + 8);
    this->state[11] = load32_le(key + 12);
    this->state[12] = load32_le(key + 16);
    this->state[13] = load32_le(key + 20);
    this->state[14] = load32_le(key + 24);
    this->state[15] = load32_le(key + 28);
}

void WukSSC::xcrypt(uint8_t *buffer, size_t length)
{
    uint32_t tmp[16] = {0};
    uint8_t *ks = (uint8_t *)tmp;

    for (size_t i = 0, ks_i = WukSSC_KSLEN; i < length; ++i, ++ks_i) {
        if (ks_i == WukSSC_KSLEN) {
            memcpy(tmp, this->state, WukSSC_KSLEN);

            // Update the keystream (10 Rounds of mixing)
            for (uint32_t j = 0; j < 5; ++j) {
                keystream_mixing(tmp);
                keystream_mixing(tmp);
            }

            // Update the state for the next keystream generation
            ++this->state[0];

            ks_i = 0;
        }

        buffer[i] ^= ks[ks_i];
    }
}

void WukSSC::xcrypt_avx2(uint8_t *buffer, size_t length) {
    const size_t BLOCK_SIZE = 1024;  // 预生成 16 个密钥块（16×64=1024 字节）
    uint32_t tmp[16];
    uint8_t ks[BLOCK_SIZE];

    // 预生成密钥流
    for (size_t ks_pos = 0; ks_pos < length; ks_pos += BLOCK_SIZE) {
        size_t block_len = MIN(BLOCK_SIZE, length - ks_pos);
        
        // 批量生成密钥流到 ks 缓冲区
        for (size_t i = 0; i < block_len; i += WukSSC_KSLEN) {
            memcpy(tmp, this->state, WukSSC_KSLEN);
            for (uint32_t j = 0; j < 5; ++j) {
                keystream_mixing(tmp);
                keystream_mixing(tmp);
            }
            memcpy(ks + i, tmp, WukSSC_KSLEN);
            ++this->state[0];
        }

        // AVX2 批量加密（一次处理 32 字节）
        size_t aligned_len = block_len - (block_len & 31);
        for (size_t i = 0; i < aligned_len; i += 32) {
            __m256i buf = _mm256_loadu_si256((__m256i*)(buffer + ks_pos + i));
            __m256i key = _mm256_loadu_si256((__m256i*)(ks + i));
            _mm256_storeu_si256((__m256i*)(buffer + ks_pos + i), _mm256_xor_si256(buf, key));
        }

        // 处理尾部（不足 32 字节部分）
        for (size_t i = aligned_len; i < block_len; ++i) {
            buffer[ks_pos + i] ^= ks[i];
        }
    }
}
