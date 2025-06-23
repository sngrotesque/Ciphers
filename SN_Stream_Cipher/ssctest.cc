#include "ssc.hh"

#include <iostream>
#include <iomanip>
#include <chrono>

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

void print_hex(const std::string& label, const uint8_t* data, size_t len) {
    std::cout << label << ": ";
    for (size_t i = 0; i < len; ++i) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

static double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void avalanche_effect_test()
{
    uint8_t left_key[WukSSC_KEYLEN]{};
    uint8_t left_nonce[WukSSC_NONCELEN]{};
    uint8_t left_counter{0};
    
    uint8_t right_key[WukSSC_KEYLEN]{};
    uint8_t right_nonce[WukSSC_NONCELEN]{1};
    uint8_t right_counter{1};

    WukSSC left_ssc(left_key, left_nonce, left_counter);
    WukSSC right_ssc(right_key, right_nonce, right_counter);

    uint8_t left_buffer[64]{};
    uint8_t right_buffer[64]{};

    for (uint32_t r = 0; r < 2; ++r) {
        std::cout << "diff state:" << std::endl;
        print_diff_hex(left_ssc.get_state(), right_ssc.get_state(), 64, 64, 16, true);

        memset(left_buffer, 0x00, 64);
        memset(right_buffer, 0x00, 64);
        left_ssc.xcrypt(left_buffer, 64);
        right_ssc.xcrypt(right_buffer, 64);

        std::cout << "diff hex:" << std::endl;
        print_diff_hex(left_buffer, right_buffer, 64, 64, 16, true);
    }
}

// 演示密钥流重用攻击
void demonstrate_key_stream_reuse_attack() {
    // 相同的密钥和nonce
    uint8_t key[WukSSC_KEYLEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    
    uint8_t nonce[WukSSC_NONCELEN] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // 明文1
    std::string plaintext1 = "This is a secret message";
    // 明文2
    std::string plaintext2 = "Attack at dawn tomorrow!";

    // 加密明文1
    WukSSC cipher1(key, nonce);
    std::string ciphertext1 = plaintext1;
    cipher1.xcrypt(reinterpret_cast<uint8_t*>(&ciphertext1[0]), ciphertext1.size());
    
    // 使用相同的密钥和nonce加密明文2
    WukSSC cipher2(key, nonce);  // 注意：重用相同的密钥和nonce
    std::string ciphertext2 = plaintext2;
    cipher2.xcrypt(reinterpret_cast<uint8_t*>(&ciphertext2[0]), ciphertext2.size());

    // 打印结果
    std::cout << "=== 密钥流重用攻击演示 ===\n";
    print_hex("明文1", reinterpret_cast<const uint8_t*>(plaintext1.data()), plaintext1.size());
    print_hex("密文1", reinterpret_cast<const uint8_t*>(ciphertext1.data()), ciphertext1.size());
    print_hex("明文2", reinterpret_cast<const uint8_t*>(plaintext2.data()), plaintext2.size());
    print_hex("密文2", reinterpret_cast<const uint8_t*>(ciphertext2.data()), ciphertext2.size());

    // 计算密文1 XOR 密文2
    std::string cipher_xor(ciphertext1.size(), '\0');
    for (size_t i = 0; i < ciphertext1.size() && i < ciphertext2.size(); ++i) {
        cipher_xor[i] = ciphertext1[i] ^ ciphertext2[i];
    }
    print_hex("密文1 XOR 密文2", reinterpret_cast<const uint8_t*>(cipher_xor.data()), cipher_xor.size());

    // 计算明文1 XOR 明文2
    std::string plain_xor(plaintext1.size(), '\0');
    for (size_t i = 0; i < plaintext1.size() && i < plaintext2.size(); ++i) {
        plain_xor[i] = plaintext1[i] ^ plaintext2[i];
    }
    print_hex("明文1 XOR 明文2", reinterpret_cast<const uint8_t*>(plain_xor.data()), plain_xor.size());

    std::cout << "\n注意：由于使用了相同的密钥流，密文1 XOR 密文2 = 明文1 XOR 明文2\n";
    std::cout << "攻击者可以利用这个性质来推断原始明文信息，特别是当知道其中一个明文时。\n";

    // 假设已知 P1
    std::string known_plaintext1 = "This is a secret message";

    // 计算 P2 = (C1 XOR C2) XOR P1
    std::string recovered_plaintext2(ciphertext1.size(), '\0');
    for (size_t i = 0; i < ciphertext1.size() && i < ciphertext2.size(); ++i) {
        recovered_plaintext2[i] = ciphertext1[i] ^ ciphertext2[i] ^ known_plaintext1[i];
    }

    std::cout << "Recovered P2: " << recovered_plaintext2 << "\n";
}

void crypto_test()
{
    uint8_t key[WukSSC_KEYLEN]{};
    uint8_t nonce[WukSSC_NONCELEN]{};

    WukSSC ssc(key, nonce);

    std::cout << "Key stream state:" << std::endl;
    print_hex(ssc.get_state(), WukSSC_KSLEN, 16, true, true);

    uint8_t buffer[256]{};
    size_t length = sizeof buffer;

    for (uint32_t i = 0; i < length; ++i) {
        buffer[i] = 0xff;
    }

    ssc.xcrypt(buffer, length);

    std::cout << "Ciphertext:" << std::endl;
    print_hex(buffer, length, 32, true, true);
}

void speed_test(size_t length = 1073741824ULL)
{
    uint8_t zero[64]{};
    WukSSC ssc(zero, zero, 0);
    uint8_t *p = new (std::nothrow) uint8_t[length];
    double start, stop;

    start = timer();
    ssc.xcrypt(p, length);
    stop = timer();
    printf("normal version Token time: %.4lf\n", stop-start);

    start = timer();
    ssc.xcrypt_avx2(p, length);
    stop = timer();
    printf("avx2 version Token time: %.4lf\n", stop-start);

    delete[] p;
}

int main()
{
    speed_test();
    // crypto_test();

    return 0;
}
