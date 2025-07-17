#include "op4/op4.hh"
#include "Test.hh"

constexpr u32 decryption_error = 777777777;
#define SI(T) static inline T

#define TEST_GB(x) x##ULL * 1024 * 1024 * 1024
#define TEST_MB(x) x##ULL * 1024 * 1024

void some_testing()
{
    u8 *key = (u8 *)"abcdef0123456789abcdef0123456789";
    u8 *nonce = (u8 *)"abcdef012345";
    u8 *iv = (u8 *)"abcdef0123456789";

    OP4 op4(key);

    const char plaintext[] = {
        "Hello, world.\n"
    };
    size_t length = strlen(plaintext);
    u8 *ciphertext = new u8[length];

    op4.ofb_xcrypt(ciphertext, (u8 *)plaintext, length, iv);

    printf("Plaintext:\t\t\t\t\t\tCiphertext:\n");
    print_diff_hex((u8 *)plaintext, ciphertext, length, length, 16, true);

    delete[] ciphertext;
}

void keystream_reuse_test()
{
    u8 *key = (u8 *)"abcdef0123456789abcdef0123456789";
    u8 *nonce = (u8 *)"abcdef012345";

    OP4 op4(key);

    size_t length = TEST_MB(1);
    u8 *plaintext = new u8[length];
    u8 *ciphertext = new u8[length];

    memset(plaintext, 0x00, length);
    memset(ciphertext, 0x00, length);

    for (size_t i = 0; i < length; i += 4096) {
        op4.ctr_xcrypt(ciphertext + i, plaintext + i, 4096, nonce);
    }

    for (size_t i = 16; i < length; i += 16) {
        int res = memcmp(ciphertext, ciphertext + i, 16);
        if (res == 0) {
            delete[] ciphertext;
            delete[] plaintext;
            std::cerr << "Position during repetition: " << i << std::endl;
            throw std::runtime_error("A key stream reuse attack has been triggered.");
        }
    }
    std::cout << "The encryption has been successfully completed." << std::endl;

    delete[] ciphertext;
    delete[] plaintext;
}

int main()
{
    try {
        keystream_reuse_test();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
