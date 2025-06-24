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

static inline void rotl128_add(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d, uint32_t shift)
{
    uint64_t high = (static_cast<uint64_t>(a) << 32) | b;
    uint64_t low  = (static_cast<uint64_t>(c) << 32) | d;

    shift &= 127;
    if (shift >= 64) {
        // 跨 64 位边界的情况
        uint64_t temp = high;
        high = low;
        low = temp;
        shift -= 64;
    }
    if (shift != 0) {
        uint64_t carry = high >> (64 - shift);
        high = (high << shift) | (low >> (64 - shift));
        low = (low << shift) | carry;
    }

    a += static_cast<uint32_t>(high >> 32);
    b += static_cast<uint32_t>(high);
    c += static_cast<uint32_t>(low >> 32);
    d += static_cast<uint32_t>(low);
}

static inline void keystream_mixing(uint32_t state[16])
{
    // Oblique mixing
    rotl128_add(state[0],  state[5],  state[10], state[15], 17);
    rotl128_add(state[4],  state[9],  state[14], state[3], 23);
    rotl128_add(state[8],  state[13], state[2],  state[7], 71);
    rotl128_add(state[12], state[1],  state[6],  state[11], 73);

    // Vertical mixing
    rotl128_add(state[0],  state[4],  state[8],  state[12], 41);
    rotl128_add(state[1],  state[5],  state[9],  state[13], 53);
    rotl128_add(state[2],  state[6], state[10],  state[14], 31);
    rotl128_add(state[3],  state[7], state[11],  state[15], 47);
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
