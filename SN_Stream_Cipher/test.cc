#include <immintrin.h>

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

static inline void rotl128_add_sse2(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d, uint32_t shift) {
    // 将4个32位整数打包到一个128位的SSE寄存器中
    __m128i vec = _mm_set_epi32(d, c, b, a);

    // 将shift限制在127以内
    shift &= 127;

    // 如果shift >= 64，交换高64位和低64位
    if (shift >= 64) {
        vec = _mm_shuffle_epi32(vec, _MM_SHUFFLE(2, 3, 0, 1));
        shift -= 64;
    }

    // 如果shift不为0，进行循环左移
    if (shift != 0) {
        __m128i high = _mm_srli_epi64(vec, 64 - shift); // 高64位右移
        __m128i low = _mm_slli_epi64(vec, shift);       // 低64位左移
        vec = _mm_or_si128(low, high);                  // 合并结果
    }

    // 将结果拆分为4个32位整数
    uint32_t result[4];
    _mm_storeu_si128((__m128i*)result, vec);

    // 与原来的值相加
    a = result[3];
    b = result[2];
    c = result[1];
    d = result[0];
}

int main()
{
    uint32_t a = 0x11111111, b = 0x22222222, c = 0x33333333, d = 0x44444444;

    rotl128_add_sse2(a, b, c, d, 16);

    printf("%08x, %08x, %08x, %08x\n", a, b, c, d);

    return 0;
}
