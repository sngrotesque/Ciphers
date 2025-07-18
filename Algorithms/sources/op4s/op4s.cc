#include "op4s/op4s.hh"

#define SI(T) static inline T
#define U32C(x) x##U

SI(constexpr u32) MUL_COEFFS[16] = {
    0xe5434f65, 0x672b6d3b, 0x73fe6c5f, 0x9d2f616b,
    0xb24113c7, 0x913131af, 0xbf674d35, 0xa0031df7,
    0x81c67423, 0x497f3f9b, 0x331119b5, 0xcc30eb31,
    0x13562dfd, 0xf57b6d83, 0x0d584be9, 0xcdcb5e63
};

SI(constexpr u32) INV_MUL_COEFFS[16] = {
    0xfb944a6d, 0xe104e3f3, 0x84f2ef9f, 0x986fc343,
    0x1384bdf7, 0x264ac54f, 0x29405d1d, 0xa560b3c7,
    0x4457db8b, 0x7c9c0e93, 0x2e4bcc9d, 0xb5668dd1,
    0xfa951755, 0x23f9092b, 0xfb8b3c59, 0xfc0e134b
};

SI(void) keystream_update(u32 state[16])
{

}

OP4S::OP4S(const u8 k[OP4S_KL], const u8 n[OP4S_NL], u32 counter)
{
    u8 count[4]{};    pack32le(count, counter);

    this->state[0]  = load32le(count);
    this->state[1]  = load32le(n);
    this->state[2]  = load32le(n + 4);
    this->state[3]  = load32le(n + 8);

    this->state[4]  = U32C(0xf6078754);
    this->state[5]  = U32C(0x35024727);
    this->state[6]  = U32C(0x07963435);
    this->state[7]  = U32C(0xe2275686);

    this->state[8]  = load32le(k);
    this->state[9]  = load32le(k + 4);
    this->state[10] = load32le(k + 8);
    this->state[11] = load32le(k + 12);
    this->state[12] = load32le(k + 16);
    this->state[13] = load32le(k + 20);
    this->state[14] = load32le(k + 24);
    this->state[15] = load32le(k + 28);
}
