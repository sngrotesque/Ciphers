from Crypto.Cipher import AES, ChaCha20_Poly1305
from Crypto.Random import get_random_bytes
from Crypto.Protocol.KDF import PBKDF2
import socket

def fwrite(fn :str, data :bytes, mode = 'wb', encoding = None):
    with open(fn, mode, encoding = encoding) as f:
        return f.write(data)

def fread(fn :str, mode = 'rb', encoding = None) -> str | bytes:
    with open(fn, mode, encoding = encoding) as f:
        return f.read()

class CipherType:
    CipherAES = 0
    CipherChaCha20 = 1

class CryptFile:
    def __init__(self, key :bytes, nonce :bytes,
                 cipher_type :CipherType = CipherType.CipherAES):
        self.key = key
        self.nonce = nonce
        self.cipher_type = cipher_type

        self.cipher = self.__aes_ctx() \
                    if cipher_type == CipherType.CipherAES \
                    else self.__cc20_ctx()

    def __aes_ctx(self):
        return AES.new(self.key, mode = AES.MODE_GCM, nonce = self.nonce)

    def __cc20_ctx(self):
        return ChaCha20_Poly1305.new(self.key, nonce = self.nonce)

    def encrypt(self, plaintext :bytes):
        ciphertext, tag = self.cipher.encrypt_and_digest(plaintext)
        return ciphertext, tag

    def decrypt(self, ciphertext :bytes, tag :bytes):
        try:
            plaintext = self.cipher.decrypt_and_verify(ciphertext, tag)
            return plaintext
        except ValueError:
            raise ValueError('解密失败，数据无法通过验证。')

def direct_encrypt(plaintext :bytes, password :str, fn :str = None):
    salt = get_random_bytes(16)
    __content = PBKDF2(password.encode(), salt, 32 + 12, 20481)
    key = __content[:32]
    nonce = __content[32:]

    cipher = CryptFile(key, nonce)
    ciphertext, tag = cipher.encrypt(plaintext)

    encrypted_data = salt + nonce + tag + ciphertext

    if fn:
        return fwrite(fn, encrypted_data)
    return encrypted_data

def direct_decrypt(ciphertext :bytes, password :str, fn :str = None):
    encrypted_data = fread(fn) if fn else ciphertext

    salt = encrypted_data[:16]
    nonce = encrypted_data[16:28]
    tag = encrypted_data[28:44]
    ciphertext = ciphertext[44:]
    
    key = PBKDF2(password.encode(), salt, 32, 20481)

    cipher = CryptFile(key, nonce)
    plaintext = cipher.decrypt(ciphertext, tag)

    return plaintext

password = 'helloworld'
ciphertext = direct_encrypt(b'1234567', password)
ciphertext = bytearray(ciphertext)
ciphertext[-1]^=1
plaintext = direct_decrypt(ciphertext, password)

print(ciphertext)
print(plaintext)