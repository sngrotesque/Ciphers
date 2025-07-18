import random
import struct
import os

def print_hex(data :bytes, num :int, new_line :bool, indent :bool):
    for i in range(len(data)):
        if (indent and ((i % num) == 0)):
            print('\t', end='')
        print(f'{data[i]:>02x}', end='')
        print(" " if ((i + 1) % num) else "\n", end='')
    if new_line: print()

def print_box(data :bytes, num :int, new_line :bool):
    for i in range(len(data)):
        print(f'0x{data[i]:02x}', end = '')
        if (i + 1) != len(data):
            if ((i + 1) % num) == 0:
                print(',\n', end = '')
            else:
                print(', ', end = '')
        else:
            print()
    if new_line:
        print()

def multiplicative_inverse(b :int):
    if (b % 2) == 0: # 不允许偶数
        return 0
    x = b
    x *= 2 - b * x;
    x *= 2 - b * x;
    x *= 2 - b * x;
    x *= 2 - b * x;
    return x & 0xffffffff

def is_prime(n):
    if n < 2:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

mul = [
    0xe5434f65, 0x672b6d3b, 0x73fe6c5f, 0x9d2f616b,
    0xb24113c7, 0x913131af, 0xbf674d35, 0xa0031df7,
    0x81c67423, 0x497f3f9b, 0x331119b5, 0xcc30eb31,
    0x13562dfd, 0xf57b6d83, 0x0d584be9, 0xcdcb5e63,
]

inv_mul = [
    0xfb944a6d, 0xe104e3f3, 0x84f2ef9f, 0x986fc343,
    0x1384bdf7, 0x264ac54f, 0x29405d1d, 0xa560b3c7,
    0x4457db8b, 0x7c9c0e93, 0x2e4bcc9d, 0xb5668dd1,
    0xfa951755, 0x23f9092b, 0xfb8b3c59, 0xfc0e134b
]
