/**
 * @author SN-Grotesque
 * @brief FEA (Fast Encryption Algorithm)
 * 
 * @copyright Copyright (c) 2025
 * 
 * This file is part of the Fast Encryption Algorithm project.
 * 
 * Due to the implementation complexity and performance
 * disadvantages of CFB encryption mode, I will completely
 * abandon this mode.
 */
#pragma once
#include "Common.hh"
#include <stdexcept>

typedef uint8_t  u8;
typedef uint32_t u32;

constexpr u32 WukFEA_NONCELEN = 12; // FEA nonce length in bytes
constexpr u32 WukFEA_KEYLEN   = 32; // FEA key length in bytes
constexpr u32 WukFEA_BL       = 16; // FEA block length in bytes
constexpr u32 WukFEA_NR       = 4;
constexpr u32 WukFEA_RKLEN    = WukFEA_KEYLEN * WukFEA_NR;

class FEA {
private:
    u8 roundKey[WukFEA_RKLEN]{0};

public:
    FEA() = default;
    FEA(const u8 key[WukFEA_KEYLEN]);
    ~FEA();

public:
    void ecb_encrypt(u8 *buffer, size_t length);
    void ecb_decrypt(u8 *buffer, size_t length);

    void cbc_encrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL]);
    void cbc_decrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL]);

    void ofb_xcrypt(u8 *buffer, size_t length, const u8 iv[WukFEA_BL]);

    void ctr_xcrypt(u8 *buffer,  size_t length, const u8 nonce[WukFEA_NONCELEN],
                    u32 counter = 0);

public:
    const u8 *getRoundKey() const {
        return this->roundKey;
    }
};
