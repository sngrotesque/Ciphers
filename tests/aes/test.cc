#include "aes/aes.hh"
#include "Test.hh"

void xcryption_test()
{
    u8 block[32] = {
        0x00, 0x01, 0x02, 0x03, // 0,  1,  2,  3,
        0x04, 0x05, 0x06, 0x07, // 4,  5,  6,  7,
        0x08, 0x09, 0x0a, 0x0b, // 8,  9,  10, 11,
        0x0c, 0x0d, 0x0e, 0x0f, // 12, 13, 14, 15
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f
    };
    u8 buffer[32] = {0};
    const u8 *k128 = (u8 *)"0123456789abcdef";
    const u8 *k192 = (u8 *)"0123456789abcdef01234567";
    const u8 *k256 = (u8 *)"0123456789abcdef0123456789abcdef";
    AES aes128(k128, AES_KEYSIZE::AES128);
    AES aes192(k192, AES_KEYSIZE::AES192);
    AES aes256(k256, AES_KEYSIZE::AES256);

    memcpy(buffer, block, 32);
    aes128.ecb_encrypt(buffer, 32);
    std::cout << "AES128-ECB Encrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);

    aes128.ecb_decrypt(buffer, 32);
    std::cout << "AES128-ECB Decrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);

    memcpy(buffer, block, 32);
    aes192.ecb_encrypt(buffer, 32);
    std::cout << "AES192-ECB Encrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);

    aes192.ecb_decrypt(buffer, 32);
    std::cout << "AES192-ECB Decrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);

    memcpy(buffer, block, 32);
    aes256.ecb_encrypt(buffer, 32);
    std::cout << "AES256-ECB Encrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);

    aes256.ecb_decrypt(buffer, 32);
    std::cout << "AES256-ECB Decrypt:" << std::endl;
    print_hex(buffer, 32, 16, true, true);
}

void round_key_test()
{
    const u8 *k256 = (u8 *)"0123456789abcdef0123456789abcdef";
    AES aes256(k256, AES_KEYSIZE::AES256);

    std::cout << "AES-256 Round key:" << std::endl;
    print_hex(aes256.get_round_key(), 240, 16, true, true);
}

void speed_test(size_t length = 512ULL * 1024 * 1024)
{
    u8 *buffer = test_alloc<u8>(length, 16);
    if (!buffer) {
        std::cerr << "new error." << std::endl;
        return;
    }
    const u8 *k128 = (u8 *)"0123456789abcdef";
    const u8 *k192 = (u8 *)"0123456789abcdef01234567";
    const u8 *k256 = (u8 *)"0123456789abcdef0123456789abcdef";
    AES aes128(k128, AES_KEYSIZE::AES128);
    AES aes192(k192, AES_KEYSIZE::AES192);
    AES aes256(k256, AES_KEYSIZE::AES256);
    double start, stop, taken_time, throughput;

    aes128.ecb_encrypt(buffer, length);
    start = timer();
    aes128.ecb_encrypt(buffer, length);
    stop = timer();
    taken_time = stop - start;
    throughput = length / taken_time / (1024 * 1024);
    printf("AES-128 Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    aes192.ecb_encrypt(buffer, length);
    start = timer();
    aes192.ecb_encrypt(buffer, length);
    stop = timer();
    taken_time = stop - start;
    throughput = length / taken_time / (1024 * 1024);
    printf("AES-192 Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    aes256.ecb_encrypt(buffer, length);
    start = timer();
    aes256.ecb_encrypt(buffer, length);
    stop = timer();
    taken_time = stop - start;
    throughput = length / taken_time / (1024 * 1024);
    printf("AES-256 Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    test_free<u8>(buffer, length, 16);
}

int main()
{
    speed_test(16 * 1024 * 1024);
    xcryption_test();
    round_key_test();

    return 0;
}
