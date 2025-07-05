#include "Common.hh"

class Caesar {
private:
    u8 k = 3;

public:
    Caesar() = default;
    Caesar(u8 k) : k(k) {};

    void encrypt(u8 *c, const u8 *p, size_t n);
    void decrypt(u8 *p, const u8 *c, size_t n);
};
