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

key = os.urandom(12)

# print_box(block, 6, True)
print_box(key,   8, True)
