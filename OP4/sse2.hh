#include <immintrin.h>

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(__GNUC__)
#   pragma GCC target("sse2,ssse3,avx2")
#endif

#ifndef WUK_SSE2
#   define WUK_SSE2              __m128i
#   define WUK_SSE2_ADD64(x, y)  _mm_add_epi64((x), (y))
#   define WUK_SSE2_ADD32(x, y)  _mm_add_epi32((x), (y))
#   define WUK_SSE2_ADD16(x, y)  _mm_add_epi16((x), (y))
#   define WUK_SSE2_ADD8(x, y)   _mm_add_epi8((x), (y))

#   define WUK_SSE2_SUB64(x, y)  _mm_sub_epi64((x), (y))
#   define WUK_SSE2_SUB32(x, y)  _mm_sub_epi32((x), (y))
#   define WUK_SSE2_SUB16(x, y)  _mm_sub_epi16((x), (y))
#   define WUK_SSE2_SUB8(x, y)   _mm_sub_epi8((x), (y))

#   define WUK_SSE2_SLLI64(x, n) _mm_slli_epi64((x), (n))
#   define WUK_SSE2_SLLI32(x, n) _mm_slli_epi32((x), (n))
#   define WUK_SSE2_SLLI16(x, n) _mm_slli_epi16((x), (n))

#   define WUK_SSE2_SRLI64(x, n) _mm_srli_epi64((x), (n))
#   define WUK_SSE2_SRLI32(x, n) _mm_srli_epi32((x), (n))
#   define WUK_SSE2_SRLI16(x, n) _mm_srli_epi16((x), (n))

#   define WUK_SSE2_ROTL32(x, n) _mm_or_si128(WUK_SSE2_SLLI32((x), (n)), WUK_SSE2_SRLI32((x), 32 - (n)))
#   define WUK_SSE2_ROTR32(x, n) _mm_or_si128(WUK_SSE2_SRLI32((x), (n)), WUK_SSE2_SLLI32((x), 32 - (n)))

#   define WUK_SSE2_AND(x, y)    _mm_and_si128((x), (y))
#   define WUK_SSE2_OR(x, y)     _mm_or_si128((x), (y))
#   define WUK_SSE2_XOR(x, y)    _mm_xor_si128((x), (y))

#   define WUK_SSE2_LOAD(x)      _mm_load_si128((__m128i *)(x))
#   define WUK_SSE2_LOADU(x)     _mm_loadu_si128((__m128i *)(x))
#   define WUK_SSE2_STORE(x, y)  _mm_store_si128((__m128i *)(x), (y))
#   define WUK_SSE2_STOREU(x, y) _mm_storeu_si128((__m128i *)(x), (y))
#endif