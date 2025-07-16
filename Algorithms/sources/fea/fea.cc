#include "fea/fea.hh"

static constexpr u8 sbox[256] = {
    // 0     1     2     3     4     5     6     7     8     9     a     n     c     d     e     f
    0x2b, 0x1b, 0xf5, 0xd5, 0x6c, 0x78, 0xe3, 0xef, 0xce, 0x69, 0xb6, 0xda, 0x28, 0x16, 0xc2, 0xbc,
    0x56, 0xe6, 0x65, 0x48, 0x6b, 0xdd, 0xa9, 0x01, 0xcb, 0x94, 0x76, 0xcf, 0x20, 0xa1, 0x19, 0x91,
    0xb1, 0xc0, 0xe7, 0x86, 0x27, 0x2e, 0xa6, 0x36, 0x95, 0xba, 0xc8, 0x0a, 0x66, 0x1f, 0x8b, 0x09,
    0x31, 0x79, 0x5b, 0xd7, 0x87, 0x81, 0x15, 0x82, 0x5f, 0x18, 0x06, 0xc1, 0xf2, 0x44, 0x9e, 0xb5,
    0x7f, 0x74, 0x0f, 0x6a, 0x52, 0x49, 0x9b, 0x70, 0x75, 0xa7, 0xbd, 0xa0, 0x0b, 0xe4, 0x32, 0xfe,
    0x35, 0x4a, 0x1a, 0x29, 0x30, 0xa5, 0x68, 0x3c, 0x4f, 0xae, 0x8c, 0x25, 0xab, 0x6d, 0xc6, 0xf7,
    0x96, 0x72, 0xd1, 0x14, 0xd8, 0x05, 0x22, 0xfc, 0x41, 0x34, 0xd6, 0xe5, 0xca, 0x55, 0x3b, 0xac,
    0xb9, 0x03, 0x11, 0x3d, 0x7d, 0xd9, 0x37, 0x7a, 0x10, 0x9a, 0xa8, 0x93, 0xdc, 0x4c, 0xe9, 0xea,
    0xfd, 0x00, 0xd4, 0xd3, 0xf9, 0x77, 0xbb, 0xeb, 0x85, 0x97, 0xbf, 0x2a, 0x53, 0x4e, 0x73, 0xec,
    0xde, 0x33, 0x57, 0x1c, 0xa3, 0x83, 0x51, 0x26, 0xa4, 0x04, 0x07, 0x5d, 0x47, 0x9c, 0x71, 0x7e,
    0x8d, 0x50, 0xcc, 0x54, 0x08, 0x98, 0x9f, 0x0d, 0xf3, 0x02, 0x7b, 0x88, 0x46, 0xb7, 0xb4, 0x9d,
    0xf1, 0x8a, 0x2d, 0x40, 0x4d, 0x5e, 0xc4, 0x12, 0xdb, 0x23, 0x6f, 0x99, 0x80, 0x64, 0x0e, 0x62,
    0x1e, 0xb2, 0x3e, 0x17, 0x3f, 0xe0, 0x39, 0xcd, 0xd2, 0xc5, 0xe2, 0x42, 0x5c, 0x89, 0x2c, 0x3a,
    0x58, 0x4b, 0xaf, 0xdf, 0xad, 0x59, 0xe8, 0x92, 0xe1, 0xa2, 0x1d, 0xaa, 0xc3, 0xf4, 0x60, 0x0c,
    0x5a, 0xed, 0x63, 0xc7, 0x38, 0x21, 0xbe, 0x61, 0x84, 0x67, 0xb8, 0xb0, 0xfb, 0x2f, 0xff, 0xb3,
    0x13, 0xfa, 0x8e, 0x45, 0x24, 0xf8, 0x6e, 0xee, 0xf0, 0x7c, 0x90, 0xd0, 0xf6, 0x43, 0x8f, 0xc9
};

static constexpr u8 rsbox[256] = {
    // 0     1     2     3     4     5     6     7     8     9     a     n     c     d     e     f
    0x81, 0x17, 0xa9, 0x71, 0x99, 0x65, 0x3a, 0x9a, 0xa4, 0x2f, 0x2b, 0x4c, 0xdf, 0xa7, 0xbe, 0x42,
    0x78, 0x72, 0xb7, 0xf0, 0x63, 0x36, 0x0d, 0xc3, 0x39, 0x1e, 0x52, 0x01, 0x93, 0xda, 0xc0, 0x2d,
    0x1c, 0xe5, 0x66, 0xb9, 0xf4, 0x5b, 0x97, 0x24, 0x0c, 0x53, 0x8b, 0x00, 0xce, 0xb2, 0x25, 0xed,
    0x54, 0x30, 0x4e, 0x91, 0x69, 0x50, 0x27, 0x76, 0xe4, 0xc6, 0xcf, 0x6e, 0x57, 0x73, 0xc2, 0xc4,
    0xb3, 0x68, 0xcb, 0xfd, 0x3d, 0xf3, 0xac, 0x9c, 0x13, 0x45, 0x51, 0xd1, 0x7d, 0xb4, 0x8d, 0x58,
    0xa1, 0x96, 0x44, 0x8c, 0xa3, 0x6d, 0x10, 0x92, 0xd0, 0xd5, 0xe0, 0x32, 0xcc, 0x9b, 0xb5, 0x38,
    0xde, 0xe7, 0xbf, 0xe2, 0xbd, 0x12, 0x2c, 0xe9, 0x56, 0x09, 0x43, 0x14, 0x04, 0x5d, 0xf6, 0xba,
    0x47, 0x9e, 0x61, 0x8e, 0x41, 0x48, 0x1a, 0x85, 0x05, 0x31, 0x77, 0xaa, 0xf9, 0x74, 0x9f, 0x40,
    0xbc, 0x35, 0x37, 0x95, 0xe8, 0x88, 0x23, 0x34, 0xab, 0xcd, 0xb1, 0x2e, 0x5a, 0xa0, 0xf2, 0xfe,
    0xfa, 0x1f, 0xd7, 0x7b, 0x19, 0x28, 0x60, 0x89, 0xa5, 0xbb, 0x79, 0x46, 0x9d, 0xaf, 0x3e, 0xa6,
    0x4b, 0x1d, 0xd9, 0x94, 0x98, 0x55, 0x26, 0x49, 0x7a, 0x16, 0xdb, 0x5c, 0x6f, 0xd4, 0x59, 0xd2,
    0xeb, 0x20, 0xc1, 0xef, 0xae, 0x3f, 0x0a, 0xad, 0xea, 0x70, 0x29, 0x86, 0x0f, 0x4a, 0xe6, 0x8a,
    0x21, 0x3b, 0x0e, 0xdc, 0xb6, 0xc9, 0x5e, 0xe3, 0x2a, 0xff, 0x6c, 0x18, 0xa2, 0xc7, 0x08, 0x1b,
    0xfb, 0x62, 0xc8, 0x83, 0x82, 0x03, 0x6a, 0x33, 0x64, 0x75, 0x0b, 0xb8, 0x7c, 0x15, 0x90, 0xd3,
    0xc5, 0xd8, 0xca, 0x06, 0x4d, 0x6b, 0x11, 0x22, 0xd6, 0x7e, 0x7f, 0x87, 0x8f, 0xe1, 0xf7, 0x07,
    0xf8, 0xb0, 0x3c, 0xa8, 0xdd, 0x02, 0xfc, 0x5f, 0xf5, 0x84, 0xf1, 0xec, 0x67, 0x80, 0x4f, 0xee
};

static inline void sub_bytes(u8 block[WukFEA_BL])
{
    for (u32 i = 0; i < WukFEA_BL; i += 4) {
        block[i    ] = sbox[block[i    ]];
        block[i + 1] = sbox[block[i + 1]];
        block[i + 2] = sbox[block[i + 2]];
        block[i + 3] = sbox[block[i + 3]];
    }
}

static inline void inv_sub_bytes(u8 block[WukFEA_BL])
{
    for (u32 i = 0; i < WukFEA_BL; i += 4) {
        block[i    ] = rsbox[block[i    ]];
        block[i + 1] = rsbox[block[i + 1]];
        block[i + 2] = rsbox[block[i + 2]];
        block[i + 3] = rsbox[block[i + 3]];
    }
}

static inline void
shift_rows( u8 &b0,  u8 &b1,  u8 &b2,  u8 &b3,
            u8 &b4,  u8 &b5,  u8 &b6,  u8 &b7,
            u8 &b8,  u8 &b9,  u8 &b10, u8 &b11,
            u8 &b12, u8 &b13, u8 &b14, u8 &b15)
{
    u32 v0, v1, v2, v3;

    v0 = load32be(b0, b1, b2, b3);
    v1 = load32be(b4, b5, b6, b7);
    v2 = load32be(b8, b9, b10, b11);
    v3 = load32be(b12, b13, b14, b15);

    v0 = rotl32(v0 + rotl32(v1, 13) + v2, 11);
    v1 = rotl32(v1 + rotl32(v2, 7)  + v3, 15);
    v2 = rotl32(v2 + rotl32(v3, 19) + v0, 23);
    v3 = rotl32(v3 + rotl32(v0, 5)  + v1, 7);

    pack32be(b0,  b1,  b2,  b3,  v0);
    pack32be(b4,  b5,  b6,  b7,  v1);
    pack32be(b8,  b9,  b10, b11, v2);
    pack32be(b12, b13, b14, b15, v3);
}

static inline void
inv_shift_rows( u8 &b0,  u8 &b1,  u8 &b2,  u8 &b3,
                u8 &b4,  u8 &b5,  u8 &b6,  u8 &b7,
                u8 &b8,  u8 &b9,  u8 &b10, u8 &b11,
                u8 &b12, u8 &b13, u8 &b14, u8 &b15)
{
    u32 v0, v1, v2, v3;

    v0 = load32be(b0, b1, b2, b3);
    v1 = load32be(b4, b5, b6, b7);
    v2 = load32be(b8, b9, b10, b11);
    v3 = load32be(b12, b13, b14, b15);

    v3 = rotr32(v3, 7)  - rotl32(v0, 5)  - v1;
    v2 = rotr32(v2, 23) - rotl32(v3, 19) - v0;
    v1 = rotr32(v1, 15) - rotl32(v2, 7)  - v3;
    v0 = rotr32(v0, 11) - rotl32(v1, 13) - v2;

    pack32be(b0,  b1,  b2,  b3,  v0);
    pack32be(b4,  b5,  b6,  b7,  v1);
    pack32be(b8,  b9,  b10, b11, v2);
    pack32be(b12, b13, b14, b15, v3);
}

static inline void Key_column_mixing(u8 k[WukFEA_BL])
{
    for (u32 i = 0; i < WukFEA_BL; i += 4) {
        k[i    ] += rotl8(k[0] ^ k[4] ^ k[8]  ^ k[12], 3);
        k[i + 1] += rotl8(k[1] ^ k[5] ^ k[9]  ^ k[13], 1);
        k[i + 2] += rotl8(k[2] ^ k[6] ^ k[10] ^ k[14], 5);
        k[i + 3] += rotl8(k[3] ^ k[7] ^ k[11] ^ k[15], 7);
    }
}

static inline void xor_with_iv(u8 block[WukFEA_BL], const u8 iv[WukFEA_BL])
{
    for(u32 i = 0; i < WukFEA_BL; i += 4) {
        block[i] ^= iv[i];
        block[i + 1] ^= iv[i + 1];
        block[i + 2] ^= iv[i + 2];
        block[i + 3] ^= iv[i + 3];
    }
}

static inline void key_extension(u8 key[WukFEA_KEYLEN], u8 roundKey[WukFEA_RKLEN])
{
    for (u32 i = 0; i < WukFEA_NR; ++i) {
        u32 offset = i * WukFEA_KEYLEN;

        // Key schedule transformation
        for (u32 r = 0; r < WukFEA_NR; ++r) {
            // This operation is to prevent users from entering keys with all zeros.
            for (u32 ki = 0; ki < WukFEA_KEYLEN; ++ki) {
                key[ki] ^= ((key[ki] >> 4) + ki * 0x1b + (key[ki] * 0x3e & 0xff));
            }

            // Really start mixing all the keys.
            sub_bytes(key);
            sub_bytes(key + WukFEA_BL);

            shift_rows(
                key[0], key[16], key[1], key[17],
                key[2], key[18], key[3], key[19],
                key[4], key[20], key[5], key[21],
                key[6], key[22], key[7], key[23]
            );
            shift_rows(
                key[8],  key[24], key[9],  key[25],
                key[10], key[26], key[11], key[27],
                key[12], key[28], key[13], key[29],
                key[14], key[30], key[15], key[31]
            );

            shift_rows(
                key[0],  key[31], key[1],  key[30],
                key[2],  key[29], key[3],  key[28],
                key[4],  key[27], key[5],  key[26],
                key[6],  key[25], key[7],  key[24]
            );
            shift_rows(
                key[8],  key[23], key[9],  key[22],
                key[10], key[21], key[11], key[20],
                key[12], key[19], key[13], key[18],
                key[14], key[17], key[15], key[16]
            );

            Key_column_mixing(key);
            Key_column_mixing(key + WukFEA_BL);
        }

        for (u32 j = 0; j < WukFEA_KEYLEN; ++j) {
            roundKey[offset + j] = key[j];
        }
    }
}

inline void cipher(u8 block[WukFEA_BL],
             const u8 roundKey[WukFEA_RKLEN])
{
    for (u32 i = 0; i < WukFEA_NR * 2; ++i) {
        u32 offset = i * WukFEA_BL;

        sub_bytes(block);
        shift_rows(
            block[0],  block[4],  block[8],  block[12],
            block[1],  block[5],  block[9],  block[13],
            block[2],  block[6],  block[10], block[14],
            block[3],  block[7],  block[11], block[15]
        );

        xor_with_iv(block, roundKey + offset);
    }
}

inline void inv_cipher(u8 block[WukFEA_BL],
                 const u8 roundKey[WukFEA_RKLEN])
{
    for (u32 i = WukFEA_NR * 2; i-- > 0;) {
        u32 offset = i * WukFEA_BL;

        xor_with_iv(block, roundKey + offset);

        inv_shift_rows(
            block[0],  block[4],  block[8],  block[12],
            block[1],  block[5],  block[9],  block[13],
            block[2],  block[6],  block[10], block[14],
            block[3],  block[7],  block[11], block[15]
        );
        inv_sub_bytes(block);
    }
}

FEA::FEA(const u8 key[WukFEA_KEYLEN])
{
    if(!key) {
        throw std::runtime_error("FEA::fea: key or iv is NULL.");
    }
    u8 copy_key[WukFEA_KEYLEN]{};
    memcpy(copy_key, key, WukFEA_KEYLEN);
    key_extension(copy_key, this->roundKey);
    memset(copy_key, 0, WukFEA_KEYLEN); // Clear sensitive data
}

FEA::~FEA()
{
    memset(this->roundKey, 0, WukFEA_RKLEN); // Clear sensitive data
}

void FEA::ecb_encrypt(u8 *buffer, size_t length)
{
    if(!buffer) {
        throw std::runtime_error("FEA::ecb_encrypt: buffer is NULL.");
    }
    if(length % WukFEA_BL != 0) {
        throw std::runtime_error("FEA::ecb_encrypt: length must be a multiple of block length.");
    }

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        cipher(buffer + i, this->roundKey);
    }
}

void FEA::ecb_decrypt(u8 *buffer, size_t length)
{
    if(!buffer) {
        throw std::runtime_error("FEA::ecb_decrypt: buffer is NULL.");
    }
    if(length % WukFEA_BL != 0) {
        throw std::runtime_error("FEA::ecb_decrypt: length must be a multiple of block length.");
    }

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        inv_cipher(buffer + i, this->roundKey);
    }
}

void FEA::cbc_encrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL])
{
    if(!buffer || !iv) {
        throw std::runtime_error("FEA::cbc_encrypt: buffer or iv is NULL.");
    }
    if(length % WukFEA_BL != 0) {
        throw std::runtime_error("FEA::cbc_encrypt: length must be a multiple of block length.");
    }

    u8 prevCipherBlock[WukFEA_BL]{};
    memcpy(prevCipherBlock, iv, WukFEA_BL);

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        xor_with_iv(buffer + i, prevCipherBlock);
        cipher(buffer + i, this->roundKey);
        memcpy(prevCipherBlock, buffer + i, WukFEA_BL);
    }
}

void FEA::cbc_decrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL])
{
    if(!buffer || !iv) {
        throw std::runtime_error("FEA::cbc_decrypt: buffer or iv is NULL.");
    }
    if(length % WukFEA_BL != 0) {
        throw std::runtime_error("FEA::cbc_decrypt: length must be a multiple of block length.");
    }

    u8 prevCipherBlock[WukFEA_BL]{};
    memcpy(prevCipherBlock, iv, WukFEA_BL);

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        u8 temp[WukFEA_BL];
        memcpy(temp, buffer + i, WukFEA_BL);
        inv_cipher(buffer + i, this->roundKey);
        xor_with_iv(buffer + i, prevCipherBlock);
        memcpy(prevCipherBlock, temp, WukFEA_BL);
    }
}

void FEA::ofb_xcrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL])
{
    if(!buffer || !iv) {
        throw std::runtime_error("FEA::ofb_encrypt: buffer or iv is NULL.");
    }
    u8 block[WukFEA_BL]{};
    memcpy(block, iv, WukFEA_BL);

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        cipher(block, this->roundKey);
        for (u32 j = 0; j < WukFEA_BL && i + j < length; ++j) {
            buffer[i + j] ^= block[j];
        }
    }
}

void FEA::ctr_xcrypt(u8 *buffer, size_t length, const u8 nonce[WukFEA_NONCELEN], u32 counter)
{
    if(!buffer || !nonce) {
        throw std::runtime_error("FEA::ctr_xcrypt: buffer or nonce is NULL.");
    }
    u8 keystream[WukFEA_BL]{};
    memcpy(keystream, nonce, WukFEA_NONCELEN);
    pack32be(keystream + WukFEA_NONCELEN, counter);

    for (size_t i = 0; i < length; i += WukFEA_BL) {
        cipher(keystream, this->roundKey);
        for (u32 j = 0; j < WukFEA_BL && i + j < length; ++j) {
            buffer[i + j] ^= keystream[j];
        }
        pack32be(keystream + WukFEA_NONCELEN, counter++);
    }
}
