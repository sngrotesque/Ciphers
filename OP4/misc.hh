#pragma once
#include <chrono>
#include <cstdint>
#include <cstdio>

void print_diff_hex_line(const uint8_t *data, size_t len, size_t start, size_t hex_per_line) {
    for (size_t j = 0; j < hex_per_line; ++j) {
        if ((start + j) < len) {
#           ifdef TEST_COLOR
            if (data[start + j] == 0) {
                printf("\x1b[91m""%02x ""\x1b[0m", data[start + j]);
            } else if (data[start + j] == 255) {
                printf("\x1b[93m""%02x ""\x1b[0m", data[start + j]);
            } else {
                printf("%02x ", data[start + j]);
            }
#           else
            printf("%02x ", data[start + j]);
#           endif
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

#       ifdef TEST_COLOR
        if (data[i] == 0) {
            printf("\x1b[91m""%02x""\x1b[0m", data[i]);
        } else if (data[i] == 255) {
            printf("\x1b[93m""%02x""\x1b[0m", data[i]);
        } else {
            printf("%02x", data[i]);
        }
#       else
        printf("%02x", data[i]);
#       endif

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
