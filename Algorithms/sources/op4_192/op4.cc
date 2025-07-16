#include "op4_192/op4.hh"

#define SI(T) static inline T

SI(const) u8 MUL_COEFFS[OP4_BL] = {
    113, 71,  97,  253, 97,  223,
    9,   53,  23,  47,  249, 15,
    5,   87,  243, 187, 203, 217,
    75,  83,  153, 11,  235, 163
};

SI(const) u8 INV_MUL_COEFFS[OP4_BL] = {
    145, 119, 161, 85,  161, 31,
    57,  29,  167, 207, 73,  239,
    205, 103, 59,  115, 227, 105,
    99,  219, 169, 163, 195, 11
};

SI(void) shift_bits_add(u8 state[OP4_BL])
{
    u32 v0, v1, v2, v3, v4, v5;

    v0 = load32le(state + 0);
    v1 = load32le(state + 4);
    v2 = load32le(state + 8);
    v3 = load32le(state + 12);
    v4 = load32le(state + 16);
    v5 = load32le(state + 20);

    v0 = rotl32(v0, 13) + v1 + v2 + v3;
    v1 = rotl32(v1, 19) + v2 + v3 + v4;
    v2 = rotl32(v2, 11) + v3 + v4 + v5;
    v3 = rotl32(v3, 17) + v4 + v5 + v0;
    v4 = rotl32(v4, 23) + v5 + v0 + v1;
    v5 = rotl32(v5, 29) + v0 + v1 + v2;

    pack32le(state,      v0);
    pack32le(state + 4,  v1);
    pack32le(state + 8,  v2);
    pack32le(state + 12, v3);
    pack32le(state + 16, v4);
    pack32le(state + 20, v5);
}

SI(void) shift_bits_sub(u8 state[OP4_BL])
{
    u32 v0, v1, v2, v3, v4, v5;

    v0 = load32le(state + 0);
    v1 = load32le(state + 4);
    v2 = load32le(state + 8);
    v3 = load32le(state + 12);
    v4 = load32le(state + 16);
    v5 = load32le(state + 20);

    v5 = rotr32(v5 - v0 - v1 - v2, 29);
    v4 = rotr32(v4 - v5 - v0 - v1, 23);
    v3 = rotr32(v3 - v4 - v5 - v0, 17);
    v2 = rotr32(v2 - v3 - v4 - v5, 11);
    v1 = rotr32(v1 - v2 - v3 - v4, 19);
    v0 = rotr32(v0 - v1 - v2 - v3, 13);

    pack32le(state + 0,  v0);
    pack32le(state + 4,  v1);
    pack32le(state + 8,  v2);
    pack32le(state + 12, v3);
    pack32le(state + 16, v4);
    pack32le(state + 20, v5);
}

SI(void) multiply(u8 state[OP4_BL])
{
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] *= MUL_COEFFS[i] & 0xff;
    }
}

SI(void) inv_multiply(u8 state[OP4_BL])
{
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] *= INV_MUL_COEFFS[i] & 0xff;
    }
}

SI(void) xor_with_iv(u8 state[OP4_BL],
                   const u8 iv[OP4_BL])
{
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] ^= iv[i];
    }
}

SI(void) xor_with_iv(u8 state[OP4_BL],
                   const u8 a[OP4_BL],
                   const u8 b[OP4_BL])
{
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] = a[i] ^ b[i];
    }
}

// Block cipher mode: OFB
SI(void) cipher(u8 state[OP4_BL],
              const u8 round_key[OP4_RKL])
{
    for (u32 r = 0; r < OP4_NR; ++r) {
        const u32 offset = OP4_BL * r;

        shift_bits_add(state);
        multiply(state);

        for (u32 i = 0; i < OP4_BL; ++i) {
            state[i] ^= round_key[offset + i];
        }
    }
}

SI(void) cipher(u8 state[OP4_BL],
          const u8 input[OP4_BL],
          const u8 round_key[OP4_RKL])
{
    memcpy(state, input, OP4_BL);

    for (u32 r = 0; r < OP4_NR; ++r) {
        const u32 offset = OP4_BL * r;

        shift_bits_add(state);
        multiply(state);

        for (u32 i = 0; i < OP4_BL; ++i) {
            state[i] ^= round_key[offset + i];
        }
    }
}

SI(void) inv_cipher(u8 state[OP4_BL],
              const u8 input[OP4_BL],
              const u8 round_key[OP4_RKL])
{
    memcpy(state, input, OP4_BL);

    for (u32 r = OP4_NR; r-- > 0; ) {
        const u32 offset = OP4_BL * r;

        for (u32 i = 0; i < OP4_BL; ++i) {
            state[i] ^= round_key[offset + i];
        }

        inv_multiply(state);
        shift_bits_sub(state);
    }
}

SI(void) prevent_zero_key(u8 key[OP4_KL])
{
    // Prevent weak keys
    for (u32 ki = 0; ki < OP4_KL; ++ki) {
        key[ki] ^= (((key[ki] + ki) - key[ki]) ^ (key[ki] << 1) ^ (key[ki] >> 4));
    }
}

SI(void) key_obfuscation(u8 k[OP4_KL])
{
    for (u32 i = 0; i < OP4_KL; i += 4) {
        k[i] += rotl8(k[i] ^ k[i+1] ^ k[i+2] ^ k[i+3], 5);
    }
    u32 v0 = load32le(k     ); u32 t0 = v0;
    u32 v1 = load32le(k +  4); u32 t1 = v1;
    u32 v2 = load32le(k +  8); u32 t2 = v2;
    u32 v3 = load32le(k + 12); u32 t3 = v3;
    u32 v4 = load32le(k + 16); u32 t4 = v4;
    u32 v5 = load32le(k + 20); u32 t5 = v5;
    u32 v6 = load32le(k + 24); u32 t6 = v6;
    u32 v7 = load32le(k + 28); u32 t7 = v7;

    t7 += rotl32((v0 ^ v7) + v6, 15);
    t6 += rotl32((v7 ^ v6) + v5, 19);
    t5 += rotl32((v6 ^ v5) + v4, 21);
    t4 += rotl32((v5 ^ v4) + v3, 29);
    t3 += rotl32((v4 ^ v3) + v2, 13);
    t2 += rotl32((v3 ^ v2) + v1, 7);
    t1 += rotl32((v2 ^ v1) + v0, 23);
    t0 += rotl32((v1 ^ v0) + v7, 17);

    pack32le(k,      t0);
    pack32le(k + 4,  t1);
    pack32le(k + 8,  t2);
    pack32le(k + 12, t3);
    pack32le(k + 16, t4);
    pack32le(k + 20, t5);
    pack32le(k + 24, t6);
    pack32le(k + 28, t7);
}

SI(void) key_schedule_transformation(u8 key[OP4_KL])
{
    for (u32 r = 0; r < OP4_NR; ++r) {
        prevent_zero_key(key);
        key_obfuscation(key);
    }
}

SI(void) key_extension(u8 key[OP4_KL], u8 round_key[OP4_RKL])
{
    for (u32 i = 0; i < OP4_NK; ++i) {
        key_schedule_transformation(key);
        memcpy(round_key + i * OP4_KL, key, OP4_KL);
    }
}

OP4::OP4(const u8 k[OP4_KL])
{
    if (!k) {
        return;
    }
    u8 tmp_k[OP4_KL]{0};
    memcpy(tmp_k, k, OP4_KL);
    key_extension(tmp_k, this->round_key);
    memset(tmp_k, 0, OP4_KL);
}

void OP4::ecb_encrypt(u8 *ciphertext, const u8 *plaintext, size_t length)
{
    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(ciphertext + i, plaintext + i, this->round_key);
    }
}

void OP4::ecb_decrypt(u8 *plaintext, const u8 *ciphertext, size_t length)
{
    for (size_t i = 0; i < length; i += OP4_BL) {
        inv_cipher(plaintext + i, ciphertext + i, this->round_key);
    }
}

void OP4::cbc_encrypt(u8 *out, const u8 *in,
                    size_t length, const u8 iv[OP4_BL])
{
    u8 buffer[OP4_BL]{0};
    memcpy(buffer, iv, OP4_BL);
    
    for (size_t i = 0; i < length; i += OP4_BL) {
        xor_with_iv(buffer, in + i);
        cipher(out + i, buffer, this->round_key);
        memcpy(buffer, out + i, OP4_BL);
    }
}

void OP4::cbc_decrypt(u8 *out, const u8 *in,
                                      size_t length, const u8 iv[OP4_BL])
{
    u8 buffer[OP4_BL]{0}, prev[OP4_BL]{0};
    memcpy(prev, iv, OP4_BL);

    for (size_t i = 0; i < length; i += OP4_BL) {
        inv_cipher(buffer, in + i, this->round_key);
        xor_with_iv(out + i, buffer, prev);
        memcpy(prev, in + i, OP4_BL);
    }
}

void OP4::ofb_xcrypt(u8 *out, const u8 *in,
                                     size_t length, const u8 iv[OP4_NL])
{
    u8 feedback[OP4_BL]{0};
    memcpy(feedback, iv, OP4_BL);

    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(feedback, this->round_key);
        xor_with_iv(out + i, in + i, feedback);
    }
}

void OP4::ctr_xcrypt(u8 *out, const u8 *in,
                                    size_t length, const u8 nonce[OP4_NL],
                                    size_t counter)
{
    u8 keystream[OP4_BL]{0x12, 0xb6, 0x36, 0x64};
    u8 keystream_state[OP4_BL]{0};
    memcpy(keystream + 4, nonce, OP4_NL);
    pack32le(keystream + 16, static_cast<u32>(counter));
    pack32le(keystream + 20, static_cast<u32>(counter >> 32));

    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(keystream_state, keystream, this->round_key);
        xor_with_iv(out + i, in + i, keystream_state);

        counter++;
        pack32le(keystream + 16, static_cast<u32>(counter));
        pack32le(keystream + 20, static_cast<u32>(counter >> 32));
    }
}
