#include "ssc.hh"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <new>

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

static double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void speed_test(size_t length = 1073741824ULL)
{
    uint8_t key[WukSSC_KEYLEN]{0};
    uint8_t nonce[WukSSC_NONCELEN]{1};
    uint32_t counter = 0;
    uint8_t *p = new (std::align_val_t(16), std::nothrow) uint8_t[length];
    uint8_t *c = new (std::align_val_t(16), std::nothrow) uint8_t[length];

    double start, stop, taken_time, throughput;
    WukSSC ssc(key, nonce, counter);

    ssc.xcrypt(c, p, length);
    start = timer();
    ssc.xcrypt(c, p, length);
    stop = timer();
    taken_time = stop - start;
    throughput = length / taken_time / (1024 * 1024);
    printf("normal version Token time: %.4lf\n", taken_time);
    printf("Speed: %.2lf MB/s.\n", throughput);

    operator delete[] (p, std::align_val_t(16));
    operator delete[] (c, std::align_val_t(16));
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

// g++ ssc.cc ssctest.cc -O3 -Wall --std=c++17 -march=native -mtune=native -o ssc.exe && ssc.exe

int main()
{
    speed_test();
    encryption_test();

    return 0;
}
