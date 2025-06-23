#include "fea.hh"

#include <iostream>
#include <iomanip>
#include <chrono>

typedef uint8_t  u8;
typedef uint32_t u32;

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

static inline double timer()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void speed_test(const u8 *key, const u8 *nonce)
{
    size_t length = 1024ULL * 1024 * 1024;
    u8 *buffer = new u8[length];

    FEA fea(key);

    double start_time = timer();
    fea.ctr_xcrypt(buffer, length, nonce);
    double end_time = timer();
    std::cout << "Encrypt Time: " << (end_time - start_time) << " seconds" << std::endl;

    delete[] buffer;
}

void encryption_test(const u8 *key, const u8 *nonce)
{
    u8 buffer[41]{0};
    size_t length = sizeof(buffer);

    FEA fea(key);
    fea.ctr_xcrypt(buffer, length, nonce);

    std::cout << "Encrypted Text:" << std::endl;
    print_hex(buffer, length, 16, true, true);
}

int main()
{
    

    return 0;
}
