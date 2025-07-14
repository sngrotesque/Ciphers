#include "caesar/caesar.hh"

// ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef
// ghijklmnopqrstuvwxyz0123456789!"
// #$%&'()*+,-./:;<=>?@[\]^_`{|}~ \n
static inline constexpr u8 table[96] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
    0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x21, 0x22,
    0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
    0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x3a, 0x3b, 0x3c,
    0x3d, 0x3e, 0x3f, 0x40, 0x5b, 0x5c, 0x5d, 0x5e,
    0x5f, 0x60, 0x7b, 0x7c, 0x7d, 0x7e, 0x20, 0x0a
};

static inline constexpr u8 discrete_table[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,  10,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static inline bool is_table(u8 c)
{
    return discrete_table[c] != 0;
}

void Caesar::encrypt(u8 *c, const u8 *p, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if (!is_table(p[i])) {
            return;
        }

        size_t idx = 0;
        while (idx < sizeof(table) && table[idx] != p[i]) {
            ++idx;
        }

        if (idx == sizeof(table)) {
            return;
        }

        c[i] = table[(idx + k) % sizeof(table)];
    }
}

void Caesar::decrypt(u8 *p, const u8 *c, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        if (!is_table(c[i])) {
            return;
        }

        // 找到 c[i] 在 table 中的索引
        size_t idx = 0;
        while (idx < sizeof(table) && table[idx] != c[i]) {
            ++idx;
        }

        if (idx == sizeof(table)) {
            return; // 未找到，非法字符
        }

        // 向左偏移 k，并取模
        p[i] = table[(idx + sizeof(table) - k) % sizeof(table)];
    }
}
