#include "caesar/caesar.hh"
#include "Test.hh"

#include <stdio.h>

int main()
{
    Caesar caesar(13);

    char p[] = {"I'm SN-Grotesque! this is Caesar cipher!"};
    constexpr size_t n = sizeof(p) - 1;
    u8 c[n] = {0};

    caesar.encrypt(c, (u8 *)p, n);

    print_hex(c, n, 16, true, false);
    printf("%s\n", c);

    caesar.decrypt((u8 *)p, c, n);

    print_hex((u8 *)p, n, 16, true, false);
    printf("%s\n", p);

    return 0;
}
