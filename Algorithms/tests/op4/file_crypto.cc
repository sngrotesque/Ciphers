#include "op4/op4.hh"
#include "Test.hh"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace fs = std::filesystem;

constexpr u32 OP4_SALT_LEN  = OP4_BL;
constexpr u32 OP4_NONCE_LEN = OP4_NL;
constexpr u32 PBKDF2_ROUNDS = 114514;
constexpr u32 block_size    = 4096;

void derive_key_pbkdf2(const char *password, const u8 salt[OP4_SALT_LEN], u8 out_key[OP4_KL])
{
    PKCS5_PBKDF2_HMAC(password, strlen(password),
                      salt, OP4_SALT_LEN,
                      PBKDF2_ROUNDS,
                      EVP_sha256(),
                      OP4_KL, out_key);
}

void file_encrypt(fs::path input_file, fs::path output_file, const char *password)
{
    u8 salt[OP4_SALT_LEN];
    u8 nonce[OP4_NONCE_LEN];
    u8 key[OP4_KL];

    RAND_bytes(salt, sizeof salt);
    RAND_bytes(nonce, sizeof nonce);
    derive_key_pbkdf2(password, salt, key);

    std::ifstream fin(input_file, std::ios::binary);
    std::ofstream fout(output_file, std::ios::binary);
    if (!fin || !fout) throw std::runtime_error("file open failed");

    // 写入 salt 和 nonce 到输出文件头部
    fout.write((char*)salt, OP4_SALT_LEN);
    fout.write((char*)nonce, OP4_NONCE_LEN);

    OP4 op4(key);

    printf("Round key:\n");
    print_hex(op4.get_rk(), OP4_RKL, 16, true, true);

    u8 plaintext[block_size]{};
    u8 ciphertext[block_size]{};
    while (fin.read((char*)plaintext, block_size) || fin.gcount()) {
        size_t n = fin.gcount();
        op4.ctr_stream(ciphertext, plaintext, n, nonce);
        fout.write((char*)ciphertext, n);
    }
}

void file_decrypt(fs::path input_file, fs::path output_file, const char *password)
{
    std::ifstream fin(input_file, std::ios::binary);
    std::ofstream fout(output_file, std::ios::binary);
    if (!fin || !fout) throw std::runtime_error("file open failed");

    u8 salt[OP4_SALT_LEN];
    u8 nonce[OP4_NONCE_LEN];
    u8 key[OP4_KL];

    // 从加密文件头部读取 salt 和 nonce
    fin.read((char*)salt, OP4_SALT_LEN);
    fin.read((char*)nonce, OP4_NONCE_LEN);
    derive_key_pbkdf2(password, salt, key);

    OP4 op4(key);
    u8 ciphertext[block_size]{};
    u8 plaintext[block_size]{};
    while (fin.read((char*)ciphertext, block_size) || fin.gcount()) {
        size_t n = fin.gcount();
        op4.ctr_stream(plaintext, ciphertext, n, nonce);
        fout.write((char*)plaintext, n);
    }
}

int main()
{
    const char *password = "12345678"; // 可由命令行或用户输入设置

    fs::path fi = L"resources/bilibili_2233.jpg";
    fs::path fo = L"resources/encrypted.op4_bin";
    fs::path fo2 = L"resources/decrypted.op4_bin";

    std::cout << "encryption..." << std::endl;
    file_encrypt(fi, fo, password);   // 加密
    std::cout << "decryption..." << std::endl;
    file_decrypt(fo, fo2, password);  // 解密（会读取 salt 和 nonce）

    return 0;
}
