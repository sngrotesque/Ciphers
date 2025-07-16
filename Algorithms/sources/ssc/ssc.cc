#include "ssc/ssc.hh"

#define U32C(x) x##U

static inline void rotl128_add(u32 &a, u32 &b, u32 &c, u32 &d, u32 shift)
{
    u64 high = (static_cast<u64>(a) << 32) | b;
    u64 low  = (static_cast<u64>(c) << 32) | d;

    if (shift >= 64) {
        // 跨 64 位边界的情况
        u64 temp = high;
        high = low;
        low = temp;
        shift -= 64;
    }
    if (shift != 0) {
        u64 carry = high >> (64 - shift);
        high = (high << shift) | (low >> (64 - shift));
        low = (low << shift) | carry;
    }

    a += static_cast<u32>(high >> 32);
    b += static_cast<u32>(high);
    c += static_cast<u32>(low >> 32);
    d += static_cast<u32>(low);
}

WukSSC::WukSSC(const u8 key[WukSSC_KEYLEN], const u8 nonce[WukSSC_NONCELEN], u32 counter)
{
    this->init(key, nonce, counter);
}

void WukSSC::init(const u8 key[WukSSC_KEYLEN], const u8 nonce[WukSSC_NONCELEN], u32 counter)
{
    u8 count[4]{};
    pack32le(count, counter);

    this->state[0]  = load32le(count);
    this->state[1]  = load32le(nonce);
    this->state[2]  = load32le(nonce + 4);
    this->state[3]  = load32le(nonce + 8);

    this->state[4]  = U32C(0xf6078754);
    this->state[5]  = U32C(0x35024727);
    this->state[6]  = U32C(0x07963435);
    this->state[7]  = U32C(0xe2275686);

    this->state[8]  = load32le(key);
    this->state[9]  = load32le(key + 4);
    this->state[10] = load32le(key + 8);
    this->state[11] = load32le(key + 12);
    this->state[12] = load32le(key + 16);
    this->state[13] = load32le(key + 20);
    this->state[14] = load32le(key + 24);
    this->state[15] = load32le(key + 28);
}

void WukSSC::xcrypt(u8 *ciphertext, const u8 *plaintext, size_t length)
{
    u32 tmp[16] = {0};
    u8 *ks = reinterpret_cast<u8 *>(tmp);

#   ifndef BLOCK_METHOD
    for (size_t i = 0, ks_i = WukSSC_KSLEN; i < length; ++i, ++ks_i) {
        if (ks_i == WukSSC_KSLEN) {
            memcpy(tmp, this->state, WukSSC_KSLEN);

            // Update the keystream (10 Rounds of mixing)
            for (u32 j = 0; j < 10; ++j) {
                // Oblique mixing
                rotl128_add(tmp[0],  tmp[5],  tmp[10], tmp[15], 17);
                rotl128_add(tmp[4],  tmp[9],  tmp[14], tmp[3],  23);
                rotl128_add(tmp[8],  tmp[13], tmp[2],  tmp[7],  71);
                rotl128_add(tmp[12], tmp[1],  tmp[6],  tmp[11], 73);
                // Vertical mixing
                rotl128_add(tmp[0],  tmp[4],  tmp[8],  tmp[12], 41);
                rotl128_add(tmp[1],  tmp[5],  tmp[9],  tmp[13], 53);
                rotl128_add(tmp[2],  tmp[6], tmp[10],  tmp[14], 31);
                rotl128_add(tmp[3],  tmp[7], tmp[11],  tmp[15], 47);
            }
            this->state[0]++; // Update the state for the next keystream generation

            ks_i = 0;
        }

        ciphertext[i] = plaintext[i] ^ ks[ks_i];
    }
#   else
    for (size_t i = 0; i < length; i += WukSSC_KSLEN) {
        memcpy(tmp, this->state, WukSSC_KSLEN);

        // Update the keystream (10 Rounds of mixing)
        for (u32 j = 0; j < 10; ++j) {
            // Oblique mixing
            rotl128_add(tmp[0],  tmp[5],  tmp[10], tmp[15], 17);
            rotl128_add(tmp[4],  tmp[9],  tmp[14], tmp[3],  23);
            rotl128_add(tmp[8],  tmp[13], tmp[2],  tmp[7],  71);
            rotl128_add(tmp[12], tmp[1],  tmp[6],  tmp[11], 73);
            // Vertical mixing
            rotl128_add(tmp[0],  tmp[4],  tmp[8],  tmp[12], 41);
            rotl128_add(tmp[1],  tmp[5],  tmp[9],  tmp[13], 53);
            rotl128_add(tmp[2],  tmp[6], tmp[10],  tmp[14], 31);
            rotl128_add(tmp[3],  tmp[7], tmp[11],  tmp[15], 47);
        }

        for (u32 j = 0; (j < WukSSC_KSLEN) && (i + j < length); ++j) {
            ciphertext[i + j] = plaintext[i + j] ^ ks[j];
        }

        this->state[0]++; // Update the state for the next keystream generation
    }
#   endif
}
