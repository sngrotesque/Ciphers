#pragma once
#include <malloc.h>

#include <cstring>
#include <cstdint>
#include <cstdlib>

#if !defined(_MSC_VER) && __cplusplus < 201703L
#   include <cstdbool>
#endif

#ifdef _WIN32
#   include <windows.h>
#endif

typedef uint8_t u8;
typedef uint32_t u32;

inline void memory_zero(void *p, size_t n)
{
    memset(p, 0x00, n);
}

inline void memory_secure(void *p, size_t n)
{
#   if defined(_WIN32)
    SecureZeroMemory(p, n);
#   elif defined(__linux)
    explicit_bzero(p, n);
#   endif
}

class Counter {
private:
    u8 counter[16]{};

public:
    Counter() = default;

    Counter(const u8 nonce[12], u32 begin)
    {
        memcpy(this->counter, nonce, 12);

        this->counter[12] = (begin >> 24) & 0xff;
        this->counter[13] = (begin >> 16) & 0xff;
        this->counter[14] = (begin >> 8)  & 0xff;
        this->counter[15] = begin         & 0xff;
    }

public:
    void step_up() noexcept
    {
        u32 v = (static_cast<u32>(this->counter[15]) << 24) |
                (static_cast<u32>(this->counter[14]) << 16) |
                (static_cast<u32>(this->counter[13]) << 8)  |
                (static_cast<u32>(this->counter[12]) << 0);
        v++;
        this->counter[12] = v; v >>= 8;
        this->counter[13] = v; v >>= 8;
        this->counter[14] = v; v >>= 8;
        this->counter[15] = v;
    }

    u8 *get() noexcept
    {
        return this->counter;
    }
};

constexpr u32 FEA_KN  = 32; // FEA key length
constexpr u32 FEA_IN  = 16; // FEA IV length
constexpr u32 FEA_BL  = 16; // FEA block length
constexpr u32 FEA_NR  = 4;
constexpr u32 FEA_RKN = FEA_KN * FEA_NR;

class FEA {
private:
    u8 roundKey[FEA_RKN]{0};

public:
    void ecb_encrypt(u8 *p);
    void ecb_decrypt(u8 *c);

    void cbc_encrypt(u8 *p, size_t n, const u8 iv[FEA_BL]);
    void cbc_decrypt(u8 *c, size_t n, const u8 iv[FEA_BL]);

    void ctr_xcrypt(u8 *d, size_t n, Counter &counter);

    void cfb_encrypt(u8 *p, size_t n, const u8 iv[FEA_BL], u32 segmentSize);
    void cfb_decrypt(u8 *c, size_t n, const u8 iv[FEA_BL], u32 segmentSize);

public:
    FEA() = default;
    FEA(const u8 *key, const u8 iv[FEA_IN]);

    const u8 *get_round_key() const;
};

/////////////////////////////////////////////////////////////////////////////////

#include <numeric>
#include <algorithm>
#include <functional>

#define SI(T) static inline T

static const u8 sbox[256] = {
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

static const u8 rsbox[256] = {
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

#define ROTL8_3(x) ((((x) >> 5) | ((x) << 3)) & 0xff)
#define ROTR8_3(x) ((((x) << 5) | ((x) >> 3)) & 0xff)

#define S(x)  (sbox[(x)])
#define RS(x) (rsbox[(x)])

SI(void) sub_bytes(u8 block[FEA_BL])
{
    for(u32 i = 0; i < FEA_BL; ++i) {
        block[i] = S(block[i]);
    }
}

SI(void) inv_sub_bytes(u8 block[FEA_BL])
{
    for(u32 i = 0; i < FEA_BL; ++i) {
        block[i] = RS(block[i]);
    }
}

SI(void) shift_bits(u8 block[FEA_BL])
{
    for(u32 i = 0; i < FEA_BL; ++i) {
        block[i] = ROTL8_3(block[i]);
    }
}

SI(void) inv_shift_bits(u8 block[FEA_BL])
{
    for(u32 i = 0; i < FEA_BL; ++i) {
        block[i] = ROTR8_3(block[i]);
    }
}

SI(void) shift_rows(u8 block[FEA_BL]) {
    std::swap_ranges(block, block + 8, block + 8);
    const u8 swap = std::accumulate(block + 8, block + 16, 0, std::bit_xor<u8>());
    std::for_each(block, block + 8, [swap](u8& x) { x ^= swap; });
}

SI(void) inv_shift_rows(u8 block[FEA_BL]) {
    const u8 swap = std::accumulate(block + 8, block + 16, 0, std::bit_xor<u8>());
    std::for_each(block, block + 8, [swap](u8& x) { x ^= swap; });
    std::swap_ranges(block, block + 8, block + 8);
}

SI(void) xor_with_iv(u8 block[FEA_BL], u8 iv[FEA_IN])
{
    for(u32 i = 0; i < FEA_BL; ++i) {
        block[i] ^= iv[i];
    }
}

SI(void) cipher(u8 block[FEA_BL], const u8 roundKey[FEA_RKN])
{
    for(u32 r = 0; r < FEA_NR; ++r) {
        sub_bytes(block);
        const u8 *subkey = roundKey + r * 32;
        for(u32 i = 0; i < FEA_KN; ++i) {
            block[i&15] ^= subkey[i];
        }
        shift_rows(block);
        shift_bits(block);
    }
}

SI(void) inv_cipher(u8 block[FEA_BL], const u8 roundKey[FEA_RKN])
{
    for(u32 r = 0; r < FEA_NR; ++r) {
        inv_shift_bits(block);
        inv_shift_rows(block);
        const u8 *subkey = roundKey + ((FEA_NR-r-1) * 32);
        for(u32 i = 0; i < FEA_KN; ++i) {
            block[i&15] ^= subkey[i];
        }
        inv_sub_bytes(block);
    }
}

void key_extension(u8 rk[FEA_RKN], const u8 key[FEA_KN], const u8 iv[FEA_IN])
{
    u8 _k[FEA_KN];
    u8 _iv[FEA_IN];

    memcpy(_k, key, FEA_KN);
    memcpy(_iv, iv, FEA_IN);

    for(u32 rk_i = 0; rk_i < FEA_RKN; rk_i += FEA_KN) {
        memcpy(rk + rk_i, _k, FEA_KN);

        sub_bytes(_k);
        sub_bytes(_k + FEA_BL);

        xor_with_iv(_k, _iv);
        xor_with_iv(_k + FEA_BL, _iv);

        for(u32 i = 0; i < FEA_BL; ++i) {
            _iv[i] ^= (_k[i] ^ _k[i + 16]);
        }

        for(u32 i = 0; i < FEA_KN; ++i) {
            _k[i] ^= 
                _iv[0]  ^ _iv[1]  ^ _iv[2]  ^ _iv[3]  ^
                _iv[4]  ^ _iv[5]  ^ _iv[6]  ^ _iv[7]  ^
                _iv[8]  ^ _iv[9]  ^ _iv[10] ^ _iv[11] ^
                _iv[12] ^ _iv[13] ^ _iv[14] ^ _iv[15];
            _k[i] ^= (_iv[i & 15] + i);
        }

        sub_bytes(_iv);
        shift_bits(_iv);

        for(u32 i = 0; i < FEA_KN; ++i) {
            _k[i] ^= 
                _iv[0] ^ _iv[2]  ^ _iv[4]  ^ _iv[6] ^
                _iv[8] ^ _iv[10] ^ _iv[12] ^ _iv[14];
            _k[i] ^= 
                _iv[1] ^ _iv[3]  ^ _iv[5]  ^ _iv[7] ^
                _iv[9] ^ _iv[11] ^ _iv[13] ^ _iv[15];
        }

        shift_bits(_k);
        shift_bits(_k + FEA_BL);

        sub_bytes(_k);
        sub_bytes(_k + FEA_BL);
    }

    memory_secure(_k, FEA_KN);
    memory_secure(_iv, FEA_IN);
}

FEA::FEA(const u8 *key, const u8 iv[FEA_IN])
{
    key_extension(this->roundKey, key, iv);
}

void FEA::ecb_encrypt(u8 *p)
{
    cipher(p, this->roundKey);
}

void FEA::ecb_decrypt(u8 *c)
{
    inv_cipher(c, this->roundKey);
}

void FEA::cbc_encrypt(u8 *p, size_t n, const u8 iv[FEA_BL])
{
    u8 cp_iv[FEA_BL];

    memcpy(cp_iv, iv, FEA_BL);
    for(u32 i = 0; i < n; i += FEA_BL) {
        xor_with_iv(p + i, cp_iv);
        cipher(p + i, this->roundKey);
        memcpy(cp_iv, p + i, FEA_BL);
    }
}

void FEA::cbc_decrypt(u8 *c, size_t n, const u8 iv[FEA_BL])
{
    u8 cp_iv[FEA_BL], block[FEA_BL];

    memcpy(cp_iv, iv, FEA_BL);
    for(u32 i = 0; i < n; i += FEA_BL) {
        memcpy(block, c + i, FEA_BL);
        inv_cipher(c + i, this->roundKey);
        xor_with_iv(c + i, cp_iv);
        memcpy(cp_iv, block, FEA_BL);
    }
}

void FEA::cfb_encrypt(u8 *p, size_t n, const u8 iv[FEA_BL], u32 segment_size)
{
    u8 cp_iv[FEA_BL];
    segment_size /= 8;
    n = (n + segment_size - 1) / segment_size;

    memcpy(cp_iv, iv, FEA_BL);
    for(size_t i = 0; i < n; ++i) {
        cipher(cp_iv, this->roundKey);
        for(size_t j = 0; j < segment_size; ++j) {
            *(p + (i * segment_size + j)) ^= cp_iv[j];
        }
        memcpy(cp_iv, p + i * segment_size, segment_size);
    }
}

void FEA::cfb_decrypt(u8 *c, size_t n, const u8 iv[FEA_BL], u32 segment_size)
{
    u8 cp_iv[FEA_BL],block[FEA_BL];
    segment_size /= 8;
    n = (n + segment_size - 1) / segment_size;

    memcpy(cp_iv, iv, FEA_BL);
    for(size_t i = 0; i < n; ++i) {
        memcpy(block, c + i * segment_size, segment_size);
        cipher(cp_iv, this->roundKey);
        for(size_t j = 0; j < segment_size; ++j) {
            *(c + (i * segment_size + j)) ^= cp_iv[j];
        }
        memcpy(cp_iv, block, segment_size);
    }
}

void FEA::ctr_xcrypt(u8 *dst, size_t n, Counter &counter)
{
    u8 ks[FEA_BL]{}, *ctr = counter.get();

    for(size_t i = 0, ks_i = FEA_BL; i < n; ++i, ++ks_i) {
        if(ks_i == FEA_BL) {
            memcpy(ks, ctr, FEA_BL);
            cipher(ks, this->roundKey);

            counter.step_up();

            ks_i = 0;
        }
        dst[i] ^= ks[ks_i];
    }
}

const u8 *FEA::get_round_key() const
{
    return this->roundKey;
}

/////////////////////////////////////////////////////////////////////
#include <iostream>
#include <chrono>

static double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void print_hex(const uint8_t *data, size_t len, size_t num, bool newline, bool indent)
{
    for(size_t i = 0; i < len; ++i) {
        if(indent && ((i) % num == 0)) {
            printf("\t");
        }

        printf("%02x", data[i]);

        printf(((i + 1) % num) ? " " : "\n");
    }
    if(newline) printf("\n");
}

void print_diff_hex_line(const uint8_t *data, size_t len, size_t start, size_t hex_per_line) {
    for (size_t j = 0; j < hex_per_line; ++j) {
        if ((start + j) < len) {
            printf("%02x ", data[start + j]);
        } else {
            printf("   "); // 三个空格对齐
        }
    }
}

void print_diff_hex(const uint8_t *data1, const uint8_t *data2,
                    size_t len1, size_t len2,
                    uint32_t hex_per_line, bool indent)
{
    size_t max_len = len1 > len2 ? len1 : len2; // 取两个数据的最大长度

    for (size_t i = 0; i < max_len; i += hex_per_line) {
        if (indent) printf("\t");

        print_diff_hex_line(data1, len1, i, hex_per_line);
        printf("\t\t");
        print_diff_hex_line(data2, len2, i, hex_per_line);

        printf("\n");
    }
}

#define MSG1 "hello, this is a test string for the Fast Encryption Algorithm. "
#define MSG2 "Hello, this is a test string for the Fast Encryption Algorithm. "

void plaintext_difference_test(const u8 key[FEA_KN], const u8 iv[FEA_IN])
{
    std::cout << "Testing plaintext difference in CBC mode...\n";
    FEA fea(key, iv);

    char p1[] = MSG1, p2[] = MSG2;
    u8 *buffer1 = reinterpret_cast<u8 *>(p1), *buffer2 = reinterpret_cast<u8 *>(p2);
    size_t length1 = strlen(p1), length2 = strlen(p2);

    fea.cbc_encrypt(buffer1, length1, iv);
    fea.cbc_encrypt(buffer2, length2, iv);

    std::cout << "CBC Encrypt Result with different plaintext:\n";
    print_diff_hex(buffer1, buffer2, length1, length2, 16, true);
}

void key_difference_test(const u8 key1[FEA_KN], const u8 key2[FEA_KN], const u8 iv[FEA_IN])
{
    std::cout << "Testing key difference in CBC mode...\n";
    FEA fea1(key1, iv);
    FEA fea2(key2, iv);

    char p1[] = MSG1, p2[] = MSG1;
    u8 *buffer1 = reinterpret_cast<u8 *>(p1), *buffer2 = reinterpret_cast<u8 *>(p2);
    size_t length1 = strlen(p1), length2 = strlen(p2);

    fea1.cbc_encrypt(buffer1, length1, iv);
    fea2.cbc_encrypt(buffer2, length2, iv);

    std::cout << "CBC Encrypt Result with different keys:\n";
    print_diff_hex(buffer1, buffer2, length1, length2, 16, true);
}

void iv_difference_test(const u8 key[FEA_KN], const u8 iv1[FEA_IN], const u8 iv2[FEA_IN])
{
    std::cout << "Testing IV difference in CBC mode...\n";
    FEA fea1(key, iv1);
    FEA fea2(key, iv2);

    char p1[] = MSG1, p2[] = MSG1;
    u8 *buffer1 = reinterpret_cast<u8 *>(p1), *buffer2 = reinterpret_cast<u8 *>(p2);
    size_t length1 = strlen(p1), length2 = strlen(p2);

    fea1.cbc_encrypt(buffer1, length1, iv1);
    fea2.cbc_encrypt(buffer2, length2, iv2);

    std::cout << "CBC Encrypt Result with different IVs:\n";
    print_diff_hex(buffer1, buffer2, length1, length2, 16, true);
}

void diff_test()
{
    u8 key1[FEA_KN] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20};
    u8 key2[FEA_KN] = {0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20};
    u8 iv1[FEA_IN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    u8 iv2[FEA_IN] = {0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    plaintext_difference_test(key1, iv1);
    key_difference_test(key1, key2, iv1);
    iv_difference_test(key1, iv1, iv2);
}

void speed_test(size_t length = 1024ULL * 1024 * 1024)
{
    u8 *buffer = new (std::nothrow) u8[length];
    u8 ctx[32 + 16]{};
    FEA fea(ctx, ctx+32);
    Counter counter((u8 *)"helloworld..", 123);

    fea.ctr_xcrypt(buffer, length, counter);
    double start = timer();
    fea.ctr_xcrypt(buffer, length, counter);
    double stop = timer();

    printf("Takne time: %.4lf.\n", stop-start);
    printf("Throughput: %.4lf MB/s.\n", length/(stop-start)/(1024*1024));

    delete[] buffer;
}

int main()
{
    speed_test();

    return 0;
}
