#include "op4/op4.hh"

#define SI(T) static inline T

SI(constexpr) u32 MUL_COEFFS[4] = {
    0x71e961d3U, 0x47dff135U, 0x172f4f25U, 0x49c1e3bfU
};

SI(constexpr) u32 INV_MUL_COEFFS[4] = {
    0x756e9e5bU, 0xd2b5991dU, 0x8ce434adU, 0x34640c3fU
};

#ifndef SIMD_SUPPORT
SI(void) shift_bits_add(u8 state[OP4_BL])
{
    u32 v0, v1, v2, v3;

    v0 = load32le(state);
    v1 = load32le(state + 4);
    v2 = load32le(state + 8);
    v3 = load32le(state + 12);

    v0 = rotl32(v0, 13) + v1 + v2 + v3;
    v1 = rotl32(v1, 19) + v2 + v3 + v0;
    v2 = rotl32(v2, 11) + v3 + v0 + v1;
    v3 = rotl32(v3, 17) + v0 + v1 + v2;

    pack32le(state,      v0);
    pack32le(state + 4,  v1);
    pack32le(state + 8,  v2);
    pack32le(state + 12, v3);
}

SI(void) shift_bits_sub(u8 state[OP4_BL])
{
    u32 v0, v1, v2, v3;

    v0 = load32le(state);
    v1 = load32le(state + 4);
    v2 = load32le(state + 8);
    v3 = load32le(state + 12);

    v3 = rotr32(v3 - v0 - v1 - v2, 17);
    v2 = rotr32(v2 - v3 - v0 - v1, 11);
    v1 = rotr32(v1 - v2 - v3 - v0, 19);
    v0 = rotr32(v0 - v1 - v2 - v3, 13);

    pack32le(state,      v0);
    pack32le(state + 4,  v1);
    pack32le(state + 8,  v2);
    pack32le(state + 12, v3);
}

SI(void) multiply(u8 state[OP4_BL])
{
    pack32le(state,      load32le(state)      * MUL_COEFFS[0]);
    pack32le(state + 4,  load32le(state + 4)  * MUL_COEFFS[1]);
    pack32le(state + 8,  load32le(state + 8)  * MUL_COEFFS[2]);
    pack32le(state + 12, load32le(state + 12) * MUL_COEFFS[3]);
}

SI(void) inv_multiply(u8 state[OP4_BL])
{
    pack32le(state,      load32le(state)      * INV_MUL_COEFFS[0]);
    pack32le(state + 4,  load32le(state + 4)  * INV_MUL_COEFFS[1]);
    pack32le(state + 8,  load32le(state + 8)  * INV_MUL_COEFFS[2]);
    pack32le(state + 12, load32le(state + 12) * INV_MUL_COEFFS[3]);
}
#endif

SI(void) xor_with_iv(u8 state[OP4_BL],
               const u8 iv[OP4_BL])
{
#   ifdef SIMD_SUPPORT
    __m128i src = _mm_loadu_si128((__m128i*)state);
    __m128i xor_val = _mm_loadu_si128((__m128i*)iv);
    __m128i dst = _mm_xor_si128(src, xor_val);
    _mm_storeu_si128((__m128i*)state, dst);
#   else
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] ^= iv[i];
    }
#   endif
}

SI(void) xor_with_iv(u8 state[OP4_BL],
               const u8 a[OP4_BL],
               const u8 b[OP4_BL])
{
#   ifdef SIMD_SUPPORT
    __m128i xor_a = _mm_loadu_si128((__m128i*)a);
    __m128i xor_b = _mm_loadu_si128((__m128i*)b);
    __m128i dst = _mm_xor_si128(xor_a, xor_b);
    _mm_storeu_si128((__m128i*)state, dst);
#   else
    for (u32 i = 0; i < OP4_BL; ++i) {
        state[i] = a[i] ^ b[i];
    }
#   endif
}

// Block cipher mode: OFB
SI(void) cipher(u8 state[OP4_BL],
          const u8 round_key[OP4_RKL])
{
#   ifdef SIMD_SUPPORT
    __m128i temp;
    __m128i rk[8] = {
        _mm_loadu_si128((__m128i *)(round_key + 0 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 1 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 2 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 3 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 4 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 5 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 6 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 7 * OP4_BL))
    };
    __m128i mul = _mm_loadu_si128((__m128i *)MUL_COEFFS);

    temp = _mm_loadu_si128((__m128i*)state);
    for (u32 r = 0; r < OP4_NR; ++r) {
        // shift bit add
        u32 v0 = _mm_extract_epi32(temp, 0);
        u32 v1 = _mm_extract_epi32(temp, 1);
        u32 v2 = _mm_extract_epi32(temp, 2);
        u32 v3 = _mm_extract_epi32(temp, 3);
        v0 = rotl32(v0, 13) + v1 + v2 + v3;
        v1 = rotl32(v1, 19) + v2 + v3 + v0;
        v2 = rotl32(v2, 11) + v3 + v0 + v1;
        v3 = rotl32(v3, 17) + v0 + v1 + v2;
        temp = _mm_set_epi32(v3, v2, v1, v0);
        // multiply
        temp = _mm_mullo_epi32(temp, mul);
        // round key add
        temp = _mm_xor_si128(temp, rk[r]);
    }
    _mm_storeu_si128((__m128i *)state, temp);
#   else
    for (u32 r = 0; r < OP4_NR; ++r) {
        shift_bits_add(state);
        multiply(state);
        xor_with_iv(state, round_key + OP4_BL * r);
    }
#   endif
}

SI(void) cipher(u8 state[OP4_BL],
          const u8 input[OP4_BL],
          const u8 round_key[OP4_RKL])
{
#   ifdef SIMD_SUPPORT
    __m128i temp;
    __m128i rk[8] = {
        _mm_loadu_si128((__m128i *)(round_key + 0 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 1 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 2 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 3 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 4 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 5 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 6 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 7 * OP4_BL))
    };
    __m128i mul = _mm_loadu_si128((__m128i *)MUL_COEFFS);

    temp = _mm_loadu_si128((__m128i*)input);
    for (u32 r = 0; r < OP4_NR; ++r) {
        // shift bit add
        u32 v0 = _mm_extract_epi32(temp, 0);
        u32 v1 = _mm_extract_epi32(temp, 1);
        u32 v2 = _mm_extract_epi32(temp, 2);
        u32 v3 = _mm_extract_epi32(temp, 3);
        v0 = rotl32(v0, 13) + v1 + v2 + v3;
        v1 = rotl32(v1, 19) + v2 + v3 + v0;
        v2 = rotl32(v2, 11) + v3 + v0 + v1;
        v3 = rotl32(v3, 17) + v0 + v1 + v2;
        temp = _mm_set_epi32(v3, v2, v1, v0);
        // multiply
        temp = _mm_mullo_epi32(temp, mul);
        // round key add
        temp = _mm_xor_si128(temp, rk[r]);
    }
    _mm_storeu_si128((__m128i *)state, temp);
#   else
    memcpy(state, input, OP4_BL);
    for (u32 r = 0; r < OP4_NR; ++r) {
        shift_bits_add(state);
        multiply(state);
        xor_with_iv(state, round_key + OP4_BL * r);
    }
#   endif
}

SI(void) inv_cipher(u8 state[OP4_BL],
              const u8 input[OP4_BL],
              const u8 round_key[OP4_RKL])
{
#   ifdef SIMD_SUPPORT
    __m128i temp;
    __m128i rk[8] = {
        _mm_loadu_si128((__m128i *)(round_key + 7 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 6 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 5 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 4 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 3 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 2 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 1 * OP4_BL)),
        _mm_loadu_si128((__m128i *)(round_key + 0 * OP4_BL))
    };
    __m128i inv_mul = _mm_loadu_si128((__m128i *)INV_MUL_COEFFS);

    temp = _mm_loadu_si128((__m128i *)input);
    for (u32 r = 0; r < OP4_NR; ++r) {
        // round key add
        temp = _mm_xor_si128(temp, rk[r]);
        // inv multiply
        temp = _mm_mullo_epi32(temp, inv_mul);
        // shift bits sub
        u32 v0 = _mm_extract_epi32(temp, 0);
        u32 v1 = _mm_extract_epi32(temp, 1);
        u32 v2 = _mm_extract_epi32(temp, 2);
        u32 v3 = _mm_extract_epi32(temp, 3);
        v3 = rotr32(v3 - v0 - v1 - v2, 17);
        v2 = rotr32(v2 - v3 - v0 - v1, 11);
        v1 = rotr32(v1 - v2 - v3 - v0, 19);
        v0 = rotr32(v0 - v1 - v2 - v3, 13);
        temp = _mm_set_epi32(v3, v2, v1, v0);
    }
    _mm_storeu_si128((__m128i *)state, temp);
#   else
    memcpy(state, input, OP4_BL);
    for (u32 r = OP4_NR; r-- > 0; ) {
        xor_with_iv(state, round_key + OP4_BL * r);
        inv_multiply(state);
        shift_bits_sub(state);
    }
#   endif
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
    // Process the 0, 4, 8, and 12 bytes each time.
    for (u32 i = 0; i < OP4_KL; i += 4) {
        const u8 val = k[i] ^ k[i+1] ^ k[i+2] ^ k[i+3];
        k[i] += rotl8(val, 5);
    }

    // Introduce a diffusion mechanism for key
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
    u8 clone_key[OP4_KL]{0};

    memcpy(clone_key, k, OP4_KL);
    key_extension(clone_key, this->round_key);
    memset(clone_key, 0, OP4_KL);
}

void OP4::ecb_encrypt(u8 *out, const u8 *in, size_t length)
{
    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(out + i, in + i, this->round_key);
    }
}

void OP4::ecb_decrypt(u8 *out, const u8 *in, size_t length)
{
    for (size_t i = 0; i < length; i += OP4_BL) {
        inv_cipher(out + i, in + i, this->round_key);
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

void OP4::cbc_decrypt(u8 *out, const u8 *in, size_t length, const u8 iv[OP4_BL])
{
    u8 buffer[OP4_BL]{0}, prev[OP4_BL]{0};
    memcpy(prev, iv, OP4_BL);

    for (size_t i = 0; i < length; i += OP4_BL) {
        inv_cipher(buffer, in + i, this->round_key);
        xor_with_iv(out + i, buffer, prev);
        memcpy(prev, in + i, OP4_BL);
    }
}

void OP4::ofb_xcrypt(u8 *out, const u8 *in, size_t length, const u8 iv[OP4_BL])
{
    u8 feedback[OP4_BL]{0};
    memcpy(feedback, iv, OP4_BL);

    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(feedback, feedback, this->round_key);
        xor_with_iv(out + i, in + i, feedback);
    }
}

void OP4::ctr_xcrypt(u8 *out, const u8 *in, size_t length, const u8 nonce[OP4_NL])
{
    u8 keystream[OP4_BL]{};
    u8 state[OP4_BL]{0};
    u8 *keystream_counter = keystream + OP4_NL;
    memcpy(keystream, nonce, OP4_NL);
    pack32le(keystream_counter, static_cast<u32>(this->counter));

    for (size_t i = 0; i < length; i += OP4_BL) {
        cipher(state, keystream, this->round_key);
        xor_with_iv(out + i, in + i, state);

        pack32le(keystream_counter, static_cast<u32>(++this->counter));
    }
}