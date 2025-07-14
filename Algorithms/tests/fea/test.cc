#include "fea/fea.hh"
#include "Test.hh"

typedef uint8_t  u8;
typedef uint32_t u32;

void speed_test(size_t length = 512ULL * 1024 * 1024)
{
    u8 key[WukFEA_KEYLEN]{0};
    u8 nonce[WukFEA_NONCELEN]{0};

    u8 *buffer = test_alloc<u8>(length, 16);

    FEA fea(key);

    fea.ctr_xcrypt(buffer, length, nonce);
    double start = timer();
    fea.ctr_xcrypt(buffer, length, nonce);
    double stop = timer();
    double taken_time = stop - start;
    double throughput = length / taken_time / (1024 * 1024);
    printf("Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    test_free<u8>(buffer, length, 16);
}

void encryption_test()
{
    u8 key[WukFEA_KEYLEN]{0};
    u8 nonce[WukFEA_NONCELEN]{0};

    u8 buffer[41]{0};
    size_t length = sizeof(buffer);

    std::cout << "Plaintext:" << std::endl;
    print_hex(buffer, length, 16, true, true);

    FEA fea(key);
    fea.ctr_xcrypt(buffer, length, nonce);

    std::cout << "Ciphertext:" << std::endl;
    print_hex(buffer, length, 16, true, true);
}

void weak_key_test()
{
    u8 key_l[WukFEA_KEYLEN] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    u8 key_r[WukFEA_KEYLEN] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    for (u32 r = 0; r < 8; ++r) {
        key_r[0] >>= 1;

        FEA fea_l(key_l);
        FEA fea_r(key_r);

        const u8 *kl = fea_l.getRoundKey();
        const u8 *kr = fea_r.getRoundKey();

        print_diff_hex(kl, kr, WukFEA_RKLEN, WukFEA_RKLEN, 16, true);

        if (memcmp(kl, kr, WukFEA_RKLEN) == 0) {
            std::cout << "Weak key has been formed!\n";
            exit(1);
        } else {
            std::cout << "The key extension algorithm is secure.\n";
        }
    }
}

int main()
{
    weak_key_test();
    // speed_test();
    // encryption_test();

    return 0;
}
