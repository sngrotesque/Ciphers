#include "ssc/ssc.hh"
#include "Test.hh"

void speed_test(size_t length = 512ULL * 1024 * 1024)
{
    uint8_t key[WukSSC_KEYLEN]{0};
    uint8_t nonce[WukSSC_NONCELEN]{1};
    uint32_t counter = 0;
    u8 *p = test_alloc<u8>(length, 16);
    u8 *c = test_alloc<u8>(length, 16);

    double start, stop, taken_time, throughput;
    WukSSC ssc(key, nonce, counter);

    ssc.xcrypt(c, p, length);
    start = timer();
    ssc.xcrypt(c, p, length);
    stop = timer();
    taken_time = stop - start;
    throughput = length / taken_time / (1024 * 1024);
    printf("Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    test_free<u8>(c, length, 16);
    test_free<u8>(p, length, 16);
}

void encryption_test()
{
    const char p[] = {
        "I'm SN-Grotesque, this is my encryption algorithm.\n"
        "It is a stream cipher algorithm that works by mixing "
        "four words to generate a keystream."
    };
    constexpr size_t n = sizeof(p) - 1;
    uint8_t c[n];

    memcpy(c, p, n);

    uint8_t key[WukSSC_KEYLEN]{0};
    uint8_t nonce[WukSSC_NONCELEN]{1};
    uint32_t counter = 0;

    WukSSC ssc(key, nonce, counter);

    ssc.xcrypt(c, (uint8_t *)p, n);

    std::cout << "Plaintext:\t\t\t\t\t\t\tCiphertext:" << std::endl;
    print_diff_hex((uint8_t *)p, c, n, n, 16, true);
}

void weak_key_test()
{
    u8 key_l[WukSSC_KEYLEN] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    u8 key_r[WukSSC_KEYLEN] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        // 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        // 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        // 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        // 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
    };
    u8 nonce[WukSSC_NONCELEN]{0};
    u8 ks_l[WukSSC_KSLEN]{0};
    u8 ks_r[WukSSC_KSLEN]{0};

    WukSSC ssc_l(key_l, nonce);
    WukSSC ssc_r(key_r, nonce);

    std::cout << "Keystream Left\t\t\t\t\t\t\t\tKeystream Right:\n";
    print_diff_hex(ssc_l.get_state(), ssc_r.get_state(), WukSSC_KSLEN, WukSSC_KSLEN, 16, true);

    ssc_l.xcrypt(ks_l, ks_l, WukSSC_KSLEN);
    ssc_r.xcrypt(ks_r, ks_r, WukSSC_KSLEN);

    std::cout << "State Left\t\t\t\t\t\t\t\tState Right:\n";
    print_diff_hex(ks_l, ks_r, WukSSC_KSLEN, WukSSC_KSLEN, 16, true);

    if (memcmp(ks_l, ks_r, WukSSC_KSLEN) == 0) {
        std::cout << "Weak key has been formed!\n";
    } else {
        std::cout << "The key extension algorithm is secure.\n";
    }
}

int main()
{
    weak_key_test();
    // speed_test();
    // encryption_test();

    return 0;
}
