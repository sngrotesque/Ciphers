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

void print_hex(const std::string& label, const uint8_t* data, size_t len) {
    std::cout << label << ": ";
    for (size_t i = 0; i < len; ++i) {
        std::cout << std::hex
                  << std::setw(2)
                  << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << "\n";
}

static double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void speed_test(size_t length = 1073741824ULL)
{
    uint8_t zero[64]{};
    WukSSC ssc(zero, zero, 0);
    uint8_t *p = new (std::align_val_t(32), std::nothrow) uint8_t[length];
    double start, stop;

    ssc.xcrypt(p, length);
    start = timer();
    ssc.xcrypt(p, length);
    stop = timer();
    printf("normal version Token time: %.4lf\n", stop-start);

    operator delete[] (p, std::align_val_t(32));
}

void encryption_test()
{
    alignas(32) char p[] = "The quick brown fox jumps over the lazy dog. Encrypt me now!!!!!";
    size_t n = sizeof(p) - 1;
    uint8_t *b = (uint8_t *)p;

    uint8_t key[WukSSC_KEYLEN]{2};
    uint8_t nonce[WukSSC_NONCELEN]{2};
    uint32_t counter = 0xfffffffe;

    WukSSC ssc(key, nonce, counter);

    ssc.xcrypt(b, n);

    print_hex(b, n, 32, false, false);
}

int main()
{
    // speed_test();
    encryption_test();

    return 0;
}
