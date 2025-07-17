#include "op4/op4.hh"
#include "Test.hh"

#include <iostream>

#define ECB_TEST
#define CBC_TEST
#define OFB_TEST
#define CTR_TEST

constexpr u32 decryption_error = 777777777;
constexpr u32 aligned_size = OP4_BL * 4;

void weak_key_test()
{
    u8 key_l[OP4_KL] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    u8 key_r[OP4_KL] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    OP4 op4_l(key_l);
    OP4 op4_r(key_r);

    const u8 *kl = op4_l.get_rk();
    const u8 *kr = op4_r.get_rk();

    std::cout << "Round key (left):\t\t\t\t\t\tRound key (Right):\n";
    print_diff_hex(kl, kr, OP4_RKL, OP4_RKL, OP4_BL, true);

    if (memcmp(kl, kr, OP4_RKL) == 0) {
        std::cout << "Weak key has been formed!\n";
        exit(decryption_error);
    } else {
        std::cout << "The key extension algorithm is secure.\n\n";
    }
}

void xcryption_verification()
{
    u8 key[OP4_KL]{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F
    };
    u8 iv[OP4_BL]{
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    u8 nonce[OP4_NL]{
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    constexpr size_t length = OP4_BL << 1;
    u8 plaintext[length]{
        0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01
    };
    u8 ciphertext[length]{0};
    u8 decrypted[length]{0};
    OP4 op4(key);

    std::cout << "Master key:\n";
    print_hex(key, OP4_KL, OP4_BL, true, true);

    std::cout << "Master Nonce:\n";
    print_hex(nonce, OP4_NL, OP4_BL, true, true);

    std::cout << "Master IV:\n";
    print_hex(iv, OP4_BL, OP4_BL, true, true);

    std::cout << "Round key\n";
    print_hex(op4.get_rk(), OP4_RKL, OP4_BL, true, true);

    std::cout << "Plaintext\n";
    print_hex(plaintext, length, OP4_BL, true, true);

#   ifdef ECB_TEST
    op4.ecb_encrypt(ciphertext, plaintext, length);
    std::cout << "\x1b[92m" << "ECB Ciphertext\n" << "\x1b[0m";
    print_hex(ciphertext, length, OP4_BL, true, true);

    op4.ecb_decrypt(decrypted, ciphertext, length);
    if (memcmp(plaintext, decrypted, length) != 0) {
        std::cout << "\x1b[91m" << "[!] ECB Decryption failed! [!]\n" << "\x1b[0m";
        exit(decryption_error);
    }
#   endif

#   ifdef CBC_TEST
    op4.cbc_encrypt(ciphertext, plaintext, length, iv);
    std::cout << "\x1b[94m" << "CBC Ciphertext\n" << "\x1b[0m";
    print_hex(ciphertext, length, OP4_BL, true, true);

    op4.cbc_decrypt(decrypted, ciphertext, length, iv);
    if (memcmp(plaintext, decrypted, length) != 0) {
        std::cout << "\x1b[91m" << "[!] CBC Decryption failed! [!]\n" << "\x1b[0m";
        exit(decryption_error);
    }
#   endif

#   ifdef OFB_TEST
    op4.ofb_xcrypt(ciphertext, plaintext, length, iv);
    std::cout << "\x1b[95m" << "OFB Ciphertext\n" << "\x1b[0m";
    print_hex(ciphertext, length, OP4_BL, true, true);

    op4.ofb_xcrypt(decrypted, ciphertext, length, iv);
    if (memcmp(plaintext, decrypted, length) != 0) {
        std::cout << "\x1b[91m" << "[!] OFB Decryption failed! [!]\n" << "\x1b[0m";
        exit(decryption_error);
    }
#   endif

#   ifdef CTR_TEST
    op4.ctr_xcrypt(ciphertext, plaintext, length, nonce);
    std::cout << "\x1b[96m" << "CTR Ciphertext\n" << "\x1b[0m";
    print_hex(ciphertext, length, OP4_BL, true, true);

    op4.set_counter(0);
    op4.ctr_xcrypt(decrypted, ciphertext, length, nonce);
    if (memcmp(plaintext, decrypted, length) != 0) {
        std::cout << "\x1b[91m" << "[!] CTR Decryption failed! [!]\n" << "\x1b[0m";
        exit(decryption_error);
    }
#   endif
}

void speed_test(size_t length = 128ULL * 1024 * 1024)
{
    u8 *plaintext = test_alloc<u8>(length, aligned_size);
    u8 *ciphertext = test_alloc<u8>(length, aligned_size);
    u8 key[OP4_KL]{0};
    u8 iv[OP4_BL]{0};
    OP4 op4(key);

    SPEED_TEST(op4.ecb_encrypt(ciphertext, plaintext, length));

    test_free(ciphertext, length, aligned_size);
    test_free(plaintext, length, aligned_size);
}

int main()
{
    weak_key_test();
    xcryption_verification();
    speed_test();

    return 0;
}
