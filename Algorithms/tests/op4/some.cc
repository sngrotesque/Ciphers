#include "op4/op4.hh"
#include "Test.hh"

constexpr u32 decryption_error = 777777777;
#define SI(T) static inline T

#define TEST_GB(x) x##ULL * 1024 * 1024 * 1024
#define TEST_MB(x) x##ULL * 1024 * 1024

void some_testing()
{
    u8 *key = (u8 *)"Abcdef0123456789abcdef0123456789";
    u8 *nonce = (u8 *)"abcdef012345";
    u8 *iv = (u8 *)"abcdef0123456789";

    OP4 op4(key);

    const char plaintext[] = {
        "GET / HTTP/1.1\r\n"
        "Accept: */*\r\n"
        "User-Agent: Android\r\n\r\n"
    };
    size_t length = strlen(plaintext);
    u8 *ciphertext = test_alloc<u8>(length, 16);
    if (!ciphertext) {
        std::cerr << "new error." << std::endl;
        return;
    }

    op4.ctr_stream(ciphertext, (u8 *)plaintext, length, iv);

    printf("Plaintext:\t\t\t\t\t\tCiphertext:\n");
    print_diff_hex((u8 *)plaintext, ciphertext, length, length, 16, true);

    test_free<u8>(ciphertext, length, 16);
}

int main()
{
    try {
        some_testing();
    } catch (std::bad_alloc &e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
