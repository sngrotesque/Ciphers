#include "chacha20.hh"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <new>

#include <cstring>

#ifdef TEST_LIBSODIUM
#   include <sodium.h>
#endif

void print_diff_hex_line(const uint8_t *data, size_t len, size_t start, size_t hex_per_line) {
    for (size_t j = 0; j < hex_per_line; ++j) {
        if ((start + j) < len) {
            printf("%02x ", data[start + j]);
        } else {
            printf("   ");
        }
    }
}

void print_diff_hex(const uint8_t *data1, const uint8_t *data2,
                    size_t len1, size_t len2,
                    uint32_t hex_per_line, bool indent)
{
    size_t max_len = len1 > len2 ? len1 : len2;

    for (size_t i = 0; i < max_len; i += hex_per_line) {
        if (indent) printf("\t");

        print_diff_hex_line(data1, len1, i, hex_per_line);
        printf("\t\t");
        print_diff_hex_line(data2, len2, i, hex_per_line);

        printf("\n");
    }
}

static inline double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

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

void speed_test(size_t length = 1024ULL * 1024 * 1024)
{
    u8 *plaintext = new (std::align_val_t(32), std::nothrow) u8[length];
    if (!plaintext) {
        std::cerr << "new error." << std::endl;
        return;
    }
    u8 *ciphertext = new (std::align_val_t(32), std::nothrow) u8[length];
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
    std::cout << "RFC 8439, Speed: " << throughput << " MB/s" << std::endl;
#   endif

    chacha20.crypto_stream(ciphertext, plaintext, length);
    start_time = timer();
    chacha20.crypto_stream(ciphertext, plaintext, length);
    end_time = timer();
    taken_time = end_time - start_time;
    throughput = length / taken_time / (1024 * 1024);
    std::cout << "SN_CC20, Speed: " << throughput << " MB/s" << std::endl;

    operator delete[] (ciphertext, std::align_val_t(32));
    operator delete[] (plaintext, std::align_val_t(32));
}

int main()
{
#   ifdef TEST_LIBSODIUM
    if (sodium_init() == -1) {
        std::cerr << "Libsodium initialization failed." << std::endl;
        return 1;
    }
    rfc8439_test();
#   endif
    chacha20_test();
    speed_test();

    return 9;
}
