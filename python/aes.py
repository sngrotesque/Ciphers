from Crypto.Cipher import AES

def aes_ecb_enc(data :bytes, key :bytes):
    ctx = AES.new(key, mode = AES.MODE_ECB)
    return ctx.encrypt(data)

def aes_ecb_dec(data :bytes, key :bytes):
    ctx = AES.new(key, mode = AES.MODE_ECB)
    return ctx.decrypt(data)

def print_hex(data :bytes, num :int, new_line :bool, indent :bool):
    for i in range(len(data)):
        if (indent and ((i % num) == 0)):
            print('\t', end='')
        print(f'{data[i]:>02x}', end='')
        print(" " if ((i + 1) % num) else "\n", end='')
    if new_line: print()

def weak_key_test():
    key1 = bytes([
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ])
    key2 = bytes([
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ])
    plaintext1 = bytes(32)
    plaintext2 = bytes(32)
    
    ciphertext1 = aes_ecb_enc(plaintext1, key1)
    ciphertext2 = aes_ecb_enc(plaintext2, key2)
    
    return ciphertext1, ciphertext2, ciphertext1 == ciphertext2

res = weak_key_test()

print(res)
