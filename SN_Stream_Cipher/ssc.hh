#include <cstdint>
#include <cstring>
#include <cstdlib>

#include <immintrin.h>

constexpr uint32_t WukSSC_KSLEN    = 64;
constexpr uint32_t WukSSC_KEYLEN   = 32;
constexpr uint32_t WukSSC_NONCELEN = 16;

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

class WukSSC {
private:
    uint32_t state[16]{0};

public:
    WukSSC() = default;
    WukSSC(const uint8_t key[WukSSC_KEYLEN], const uint8_t nonce[WukSSC_NONCELEN], uint32_t counter = 0);

public:
    void init(const uint8_t key[WukSSC_KEYLEN], const uint8_t nonce[WukSSC_NONCELEN], uint32_t counter = 0);
    void xcrypt(uint8_t *buffer, size_t length);
    void xcrypt_avx2(uint8_t *buffer, size_t length);

    const uint8_t *get_state() const noexcept
    {
        return reinterpret_cast<const uint8_t *>(state);
    }
};
