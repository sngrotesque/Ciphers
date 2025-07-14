/*
 * **************************************************************************
 * 此密码为纯C++实现的AES128/192/256加密算法，它仅供学习使用，虽然
 * 安全性与标准AES相同，但其性能极差（由于是纯软件实现），所以不建议使用。
 * 平均性能为55MB/s。
 * **************************************************************************
 * This cipher is a pure C++ implementation of the AES-128/192/256 encryption
 * algorithm. It is intended for educational purposes only. While its security
 * is equivalent to that of standard AES, its performance is extremely poor (due
 * to being a purely software-based implementation), and therefore it is not
 * recommended for practical use.
 * The average performance is 55 MB/s.
 * **************************************************************************
 */
#include "Common.hh"
#include <vector>

constexpr u32 AESXXX_BL = 16;
constexpr u32 AESXXX_NB = 4;

constexpr u32 AES128_NK = 4;
constexpr u32 AES128_NR = 10;

constexpr u32 AES192_NK = 6;
constexpr u32 AES192_NR = 12;

constexpr u32 AES256_NK = 8;
constexpr u32 AES256_NR = 14;

enum class AES_KEYSIZE
{
    AES128 = 16,
    AES192 = 24,
    AES256 = 32
};

class AES {
private:
    const u32 nb = AESXXX_NB;
    // default
    u32 nk = AES128_NK;
    u32 nr = AES128_NR;

    u8 round_key[AESXXX_BL * (AES256_NR + 1)]{0};
    AES_KEYSIZE key_size;

private:
    void key_expansion(const u8 *key);
    void cipher(u8 b[AESXXX_BL], const u8 *rk);
    void inv_cipher(u8 b[AESXXX_BL], const u8 *rk);

public:
    AES() = default;
    AES(const u8 *key, AES_KEYSIZE key_size);
    ~AES();

public:
    void ecb_encrypt(u8 *buffer, size_t length);
    void ecb_decrypt(u8 *buffer, size_t length);

public:
    const u8 *get_round_key() const noexcept
    {
        return this->round_key;
    }
};
