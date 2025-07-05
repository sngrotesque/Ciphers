#include "op4/op4.hh"
#include "op4/op4.cc"
#include "Test.hh"

void weak_key_test()
{
    u8 key_l[OP4_KL] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    u8 key_r[OP4_KL] = {
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    OP4 op4_l(key_l);
    OP4 op4_r(key_r);

    const u8 *kl = op4_l.get_rk();
    const u8 *kr = op4_r.get_rk();

    std::cout << "Round key (left):\t\t\t\t\t\t\t\tRound key (Right):\n";
    print_diff_hex(kl, kr, OP4_RKL, OP4_RKL, 24, true);

    if (memcmp(kl, kr, OP4_RKL) == 0) {
        std::cout << "Weak key has been formed!\n";
        exit(777777777);
    } else {
        std::cout << "The key extension algorithm is secure.\n";
    }
}

void xcryption_verification()
{
    constexpr size_t length = OP4_BL * 2;
    u8 plaintext[length]{0b00000000};
    u8 ciphertext[length]{0};
    u8 decrypted[length]{0};
    u8 key[OP4_KL]{0};
    OP4 op4(key);

    std::cout << "Round key\n";
    print_hex(op4.get_rk(), OP4_RKL, 24, true, true);

    std::cout << "Plaintext\n";
    print_hex(plaintext, length, 24, true, true);

    op4.ecb_encrypt(ciphertext, plaintext, length);
    std::cout << "Ciphertext\n";
    print_hex(ciphertext, length, 24, true, true);

    op4.ecb_decrypt(decrypted, ciphertext, length);
    std::cout << "Decrypted\n";
    print_hex(decrypted, length, 24, true, true);

    if (memcmp(plaintext, decrypted, length) != 0) {
        std::cout << "Decryption failed!\n";
    }
}

void speed_test(size_t length = 1024ULL * 1024 * 1024 + 8)
{
    u8 *plaintext = test_alloc<u8>(length, 8);
    u8 *ciphertext = test_alloc<u8>(length, 8);
    u8 key[OP4_KL]{0};
    OP4 op4(key);

    SPEED_TEST(op4.ecb_encrypt(ciphertext, plaintext, length));

    test_free(ciphertext, length, 8);
    test_free(plaintext, length, 8);
}

int main()
{
    weak_key_test();
    xcryption_verification();
    speed_test();

    return 0;
}
