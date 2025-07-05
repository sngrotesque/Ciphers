#include "chacha20/chacha20.hh"
#include "Test.hh"

#ifdef TEST_LIBSODIUM
#   include <sodium.h>
#endif

constexpr u8 nonce[12] = {1,2,3,4,5,6,7,8};
constexpr u8 key[32]   = {0,1,2,3,4,5,6,7};
constexpr u32 counter   = 123456;

#ifdef TEST_LIBSODIUM
void rfc8439_test()
{
    const char *plaintext = {
        "In a quiet village nestled between the hills, every sunrise painted "
        "the rooftops in gold, and the breeze carried laughter from the market "
        "square. Time moved slowly, and life danced gently to the rhythm of "
        "simplicity and wonder."
    };
    size_t length = strlen(plaintext);

    u8 *ciphertext = new (std::nothrow) u8[length];
    if (!ciphertext) {
        std::cerr << "new error." << std::endl;
        return;
    }

    crypto_stream_chacha20_ietf_xor_ic(ciphertext, (u8 *)plaintext, length, nonce, counter, key);

    std::cout << "Plaintext:\t\t\t\t\t\t\tCiphertext:" << std::endl;
    print_diff_hex((u8 *)plaintext, ciphertext, length, length, 16, true);

    delete[] ciphertext;
}
#endif

void chacha20_test()
{
    const char *plaintext = {
        "In a quiet village nestled between the hills, every sunrise painted "
        "the rooftops in gold, and the breeze carried laughter from the market "
        "square. Time moved slowly, and life danced gently to the rhythm of "
        "simplicity and wonder."
    };
    size_t length = strlen(plaintext);

    u8 *ciphertext = new (std::nothrow) u8[length];
    if (!ciphertext) {
        std::cerr << "new error." << std::endl;
        return;
    }

    ChaCha20 chacha20(key, nonce, counter);
    chacha20.crypto_stream(ciphertext, (u8 *)plaintext, length);

    std::cout << "Plaintext:\t\t\t\t\t\t\tCiphertext:" << std::endl;
    print_diff_hex((u8 *)plaintext, ciphertext, length, length, 16, true);

    delete[] ciphertext;
}

void speed_test(size_t length = 512ULL * 1024 * 1024)
{
    u8 *plaintext = test_alloc<u8>(length, 32);
    if (!plaintext) {
        std::cerr << "new error." << std::endl;
        return;
    }
    u8 *ciphertext = test_alloc<u8>(length, 32);
    if (!ciphertext) {
        std::cerr << "new error." << std::endl;
        return;
    }
    double start_time, end_time, taken_time, throughput;

    ChaCha20 chacha20(key, nonce, counter);

#   ifdef TEST_LIBSODIUM
    crypto_stream_chacha20_ietf_xor_ic(ciphertext, plaintext, length, nonce, counter, key);
    start_time = timer();
    crypto_stream_chacha20_ietf_xor_ic(ciphertext, plaintext, length, nonce, counter, key);
    end_time = timer();
    taken_time = end_time - start_time;
    throughput = length / taken_time / (1024 * 1024);
    printf("Token time: %.4lf\n", taken_time);
    printf("RFC 8439 Speed: %.2lf MB/s.\n", throughput);
#   endif

    chacha20.crypto_stream(ciphertext, plaintext, length);
    start_time = timer();
    chacha20.crypto_stream(ciphertext, plaintext, length);
    end_time = timer();
    taken_time = end_time - start_time;
    throughput = length / taken_time / (1024 * 1024);
    printf("Token time: %.4lf\n", taken_time);
    printf("SN-CC20 Speed: %.2lf MB/s.\n", throughput);

    test_free<u8>(ciphertext, length, 32);
    test_free<u8>(plaintext, length, 32);
}

int main()
{
    speed_test();
#   ifdef TEST_LIBSODIUM
    if (sodium_init() == -1) {
        std::cerr << "Libsodium initialization failed." << std::endl;
        return 1;
    }
    rfc8439_test();
#   endif
    chacha20_test();

    return 0;
}
