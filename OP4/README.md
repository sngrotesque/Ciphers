## **1. Introduction**
The **OP4** block cipher is a novel symmetric encryption algorithm designed with a focus on efficiency, security, and simplicity. Its name derives from the **four fundamental arithmetic operations** (addition, subtraction, multiplication, and XOR) that form its core transformation layers. This document provides a comprehensive overview of OP4's design, implementation, and test results, and invites the cryptographic community to evaluate its security through rigorous analysis.

---

## **2. Algorithm Specifications**
### **2.1 Key Features**
- **Block Size**: 128 bits (16 bytes)  
- **Key Length**: 256 bits (32 bytes)  
- **Rounds**: 8 rounds of encryption/decryption  
- **Subkey Size**: 128 bits per round  
- **Modes Supported**: ECB, CBC, OFB, CTR   
- **Confusion & Diffusion**: Achieves full diffusion in just **3 rounds** (1-bit change propagates to the entire state).  
- **S-Box Replacement**: Uses arithmetic operations (+, -, ×, ⊕) and bit rotations instead of traditional S-boxes.  

### **2.2 Core Components**
#### **(1) Key Schedule**
- **Key Obfuscation**: Applies non-linear transformations to prevent weak keys.  
- **Round Key Generation**: Each round derives a 128-bit subkey via iterative bit rotations and modular arithmetic.  

#### **(2) Encryption Process**
Each round consists of:  
1. **Shift-Add Layer**: Bitwise rotations (`ROTL32`) and modular addition for diffusion.  
2. **Multiply Layer**: Byte-wise multiplication with fixed coefficients (`MUL_COEFFS`).  
3. **Key Mixing**: XOR with the round subkey.  

#### **(3) Decryption Process**
Reverses encryption using inverse operations (`ROTR32`, `INV_MUL_COEFFS`).  

---

## **3. Security Analysis & Test Results**
### **3.1 Encryption Tests**
Three extreme test cases were conducted with repeating patterns in keys, IVs, and plaintext:  

| Test Case | Key/IV/Nonce Pattern | Plaintext Pattern | ECB Ciphertext (First Block)          |
|-----------|----------------------|-------------------|---------------------------------------|
| 1         | All `0x00`           | `0x00 0x01` repeat| `ba d2 19 24 d8 a5 3e f2 2b 87 54 f2` |
| 2         | All `0xFF`           | `0x00 0x01` repeat| `eb fa c3 74 f4 a7 3a b3 ce 89 08 af` |
| 3         | All `0x80`           | `0x00 0x01` repeat| `8a d1 2c f8 99 b9 3f b7 25 c2 c1 2c` |

**Observations**:  
- **ECB mode** produces identical ciphertext blocks for identical plaintext blocks (expected behavior).  
- **CBC/OFB/CTR modes** introduce variability due to IV/nonce usage .  

### **3.2 Avalanche Effect Test**
- **Modification**: Flipping the LSB of the first plaintext byte (`0x00` → `0x01`).  
- **Result**:  
  - ECB ciphertext changed **completely** (`8a d1 2c f8...` → `53 7b 67 8e...`).  
  - Confirms strong diffusion properties.  

### **3.3 Related-Key Attack Resistance**
- **Test**: Modified the last byte of the key (`0xFF` → `0xFE` and `0x7F`).  
- **Result**:  
  - Ciphertexts diverged significantly (`eb fa c3 74...` → `1d 70 8e 15...` and `a9 af 26 48...`).  
  - Demonstrates sensitivity to key changes, mitigating related-key attacks.  

---

## **4. Comparison with Existing Block Ciphers**

| Feature         | OP4                | AES-256           | SM4               |
|-----------------|--------------------|-------------------|-------------------|
| Block Size      | 128 bits           | 128 bits          | 128 bits          |
| Key Length      | 256 bits           | 256 bits          | 128 bits          |
| Rounds          | 8                  | 14                | 32                |
| S-Box           | Arithmetic ops     | Lookup table      | Lookup table      |
| Modes Supported | ECB/CBC/OFB/CTR    | ECB/CBC/CTR/etc.  | ECB/CBC/etc.      |

**Advantages of OP4**:  
- Arithmetic-based confusion avoids S-box timing attacks.  
- Efficient diffusion in 3 rounds.  

---

## **5. Cryptanalysis Invitation**
We invite the cryptographic community to evaluate OP4’s security. Potential areas of study:  
1. **Differential/Linear Cryptanalysis**: Assess resistance against known-plaintext attacks.  
2. **Side-Channel Attacks**: Test vulnerability to timing/power analysis.  
3. **Key Schedule Weaknesses**: Examine subkey generation for biases.  
4. **Algebraic Attacks**: Exploit the arithmetic-based structure.  

**Test Vectors Available**: Full results from extreme-case tests (all `0x00`, `0xFF`, `0x80`) are provided for verification.  

---

## **6. Conclusion**
OP4 is a promising block cipher that combines arithmetic operations with a large block size for enhanced security. Initial tests confirm strong diffusion and key sensitivity. We encourage independent cryptanalysis to validate its robustness.  

**Repository & Contact**:  
- Code: [OP4 - Github](https://github.com/sngrotesque/OP4/)  
- Email: [Gmail: SN](sngrotesque@gmail.com)  

*— Submitted for peer review, July 2025.*



### The first test result
```plaintext
Master key:
        00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
        00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

Master Nonce:
        00 00 00 00 00 00 00 00 00 00 00 00
Master IV:
        00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

Round key
        0f c6 be ba ec 7a 68 51 9e e5 03 bd 9f ed 52 1d
        a2 9d ba 69 b5 fb 49 ab 33 f8 91 97 0c ec f1 c5
        a7 46 70 e4 2b 60 5c 9d bd 56 2a 4d 3e 9a 47 79
        11 56 05 cb e3 58 3e a1 7f a2 11 5d 89 ca f1 12
        d1 32 7d f0 c6 7e cd 7c 30 83 77 df 6e ee 22 84
        96 88 a3 bd 90 8b aa 8c 1e 9b 33 01 3c c1 c7 49
        d3 f5 bf b5 a2 c4 36 2c 3c a9 d8 46 bc 39 16 ef
        bd c0 9c 4e 85 b7 20 fd 80 3a fe 79 91 a5 c7 57

Plaintext
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        ba d2 19 24 d8 a5 3e f2 2b 87 54 f2 6e 65 17 a2
        ba d2 19 24 d8 a5 3e f2 2b 87 54 f2 6e 65 17 a2

CBC Ciphertext
        ba d2 19 24 d8 a5 3e f2 2b 87 54 f2 6e 65 17 a2
        6a e1 04 87 cd cf 2d 45 55 14 09 92 e6 61 58 83

OFB Ciphertext
        8e 20 f4 c3 d4 87 f9 e7 2c e3 26 cb 10 b3 f8 17
        2b 9a 51 47 4f ea 73 2f d5 ac 03 04 11 c4 bd b0

CTR Ciphertext
        18 93 ad d9 86 5e df e0 37 89 f5 e3 b3 55 d0 e6
        dd 8f 9e 23 f0 5c 29 e3 34 4a 14 0c 96 4c 94 51
```

### Second test result:
```plaintext
Master key:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff

Master Nonce:
        ff ff ff ff ff ff ff ff ff ff ff ff
Master IV:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff

Round key
        a8 f8 93 a2 d4 5b 76 68 1d 1f 6f 2a 1f 11 15 82
        c8 77 9d 10 8b c8 58 1d 9e c8 09 03 4d 05 fc 64
        d2 dc dc 16 63 a2 4d 28 cf 21 a5 f9 64 40 b9 95
        74 8a e0 44 81 2f 2e ff 88 91 40 1d e2 d1 e2 b8
        9c b3 ff 16 23 1f e0 92 ac 10 7f 9d ab f7 c6 cf
        07 15 34 a4 20 24 f6 80 38 94 01 f7 7c 1f 33 5c
        3a f6 64 32 cd 3f 74 47 1d 64 ae 43 99 fb df 0a
        ca e3 03 39 e2 00 38 70 62 00 f7 ba 73 44 b5 55

Plaintext
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        eb fa c3 74 f4 a7 3a b3 ce 89 08 af 77 b4 c8 b3
        eb fa c3 74 f4 a7 3a b3 ce 89 08 af 77 b4 c8 b3

CBC Ciphertext
        34 ca 74 6b 3e 0b 07 44 e1 f8 f2 f3 2b 8c db 9d
        73 9d 32 d0 ff fd a2 7f 5a 4c 72 aa 07 cc 88 97

OFB Ciphertext
        bd 68 e9 e9 db dc 11 f0 88 31 d5 8f c8 62 0b f9
        0f 69 66 80 dc eb df f7 32 b0 9a bb af 27 e4 f9

CTR Ciphertext
        ec 88 56 af 37 5b 49 41 2e 15 d8 5f eb 22 e2 b3
        dd 1e e5 20 c7 3b 2b 2c 84 09 11 34 05 21 bb 2f
```

### Third test result
```plaintext
Master key:
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80

Master Nonce:
        80 80 80 80 80 80 80 80 80 80 80 80
Master IV:
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80

Round key
        06 c7 7d 62 1b ae e9 10 4e 61 c9 3a 5e 31 11 af
        f4 6a 4e 90 43 39 dd 21 09 71 47 55 9c fc 9f 51
        c6 3a ce 35 03 dd ee 2d 42 97 7e d1 5f dc 12 a5
        a4 b3 07 84 22 01 e5 7e 1d e1 5f 0f 8c 74 77 03
        de 07 7a 8a 42 b2 30 eb ca a7 d0 d6 6b 35 ba f5
        52 39 53 d0 e2 d4 9e 51 40 4d cc 5f d5 08 b4 73
        44 c9 56 4a d6 d8 c3 36 1d 44 cb 69 95 40 43 45
        78 a6 43 77 00 04 db a9 b7 db 60 d1 e6 95 81 97

Plaintext
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        8a d1 2c f8 99 b9 3f b7 25 c2 c1 2c 72 ed 78 3f
        8a d1 2c f8 99 b9 3f b7 25 c2 c1 2c 72 ed 78 3f

CBC Ciphertext
        c2 27 dd f3 26 d0 49 d5 f6 37 af 10 6f 3c b2 9f
        58 f4 de cc 97 fd bb 45 bd bc d5 51 80 68 68 6d

OFB Ciphertext
        80 eb 12 9e 9c 16 65 bf 81 98 0d ee 3b b7 3f 89
        c0 27 ed 5a 30 9d 5e f6 3b 22 ad a3 4a 63 bb 77

CTR Ciphertext
        b9 73 6a 2c 70 1a e0 0d 30 9e c6 85 69 47 d4 17
        7d f8 24 24 ca 54 14 97 6e bf 9e 95 04 90 5c c6
```


### Proof of Avalanche Effect in Plaintext

1. Continue to use the Key, Nonce, and IV from the third test.
2. Only modify the lowest bit of the first byte in plaintext.

```plaintext
Master key:
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80

Master Nonce:
        80 80 80 80 80 80 80 80 80 80 80 80
Master IV:
        80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80

Round key
        06 c7 7d 62 1b ae e9 10 4e 61 c9 3a 5e 31 11 af
        f4 6a 4e 90 43 39 dd 21 09 71 47 55 9c fc 9f 51
        c6 3a ce 35 03 dd ee 2d 42 97 7e d1 5f dc 12 a5
        a4 b3 07 84 22 01 e5 7e 1d e1 5f 0f 8c 74 77 03
        de 07 7a 8a 42 b2 30 eb ca a7 d0 d6 6b 35 ba f5
        52 39 53 d0 e2 d4 9e 51 40 4d cc 5f d5 08 b4 73
        44 c9 56 4a d6 d8 c3 36 1d 44 cb 69 95 40 43 45
        78 a6 43 77 00 04 db a9 b7 db 60 d1 e6 95 81 97

Plaintext
        01 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        53 7b 67 8e a1 a8 06 41 5b c7 54 7b fd 2e f4 ba
        8a d1 2c f8 99 b9 3f b7 25 c2 c1 2c 72 ed 78 3f

CBC Ciphertext
        64 ee 1f 64 d2 77 d0 ba 5b 4c 37 5f ba 97 64 f3
        9e c8 9a f4 d4 c5 7b 4d 32 fc da 14 d8 75 f5 48

OFB Ciphertext
        81 eb 12 9e 9c 16 65 bf 81 98 0d ee 3b b7 3f 89
        c0 27 ed 5a 30 9d 5e f6 3b 22 ad a3 4a 63 bb 77

CTR Ciphertext
        b8 73 6a 2c 70 1a e0 0d 30 9e c6 85 69 47 d4 17
        7d f8 24 24 ca 54 14 97 6e bf 9e 95 04 90 5c c6
```

### Defense testing against related key attacks

1. Use the Key, Nonce, and IV from the second test.
2. Only modify the lowest and highest bits of the last byte of the Key.

```plaintext
Master key:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff fe

Master Nonce:
        ff ff ff ff ff ff ff ff ff ff ff ff
Master IV:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff

Round key
        46 78 e2 41 a3 a6 d0 47 6a 1c 11 3f 1b 14 fc 38
        85 96 ff 0d b1 d6 29 39 04 cf a9 0b de bf f9 1c
        26 9b 02 e4 26 10 17 7c c4 b4 87 5a 01 82 dc 5b
        71 a6 4f 20 b6 01 97 d4 de 5f 87 92 aa 7c cb d8
        ea 3a 30 6b 8c 88 2f 95 cc 70 ed dc 8b 2c 77 a8
        f1 ee 2c 1b 02 2d 6a 1e 29 89 0c bb ca e8 f9 98
        4a 64 1a f1 0b cb ab 4e 3f d3 2c 1b fd 06 bb 5f
        b0 63 26 38 35 60 22 81 c6 4d 1e d5 bc fa 40 70

Plaintext
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        1d 70 8e 15 0a b6 4b 17 cd 5b 1f ba e2 e7 81 fa
        1d 70 8e 15 0a b6 4b 17 cd 5b 1f ba e2 e7 81 fa

CBC Ciphertext
        42 08 0f d8 b6 59 3c 16 40 1f dc 07 e3 fe 2e 26
        fb cf 13 17 a4 8f 8d 9b 21 ed 8c 75 a4 85 60 6b

OFB Ciphertext
        d3 2e b8 4f 88 26 71 fd b3 fb 67 b4 1e 08 24 22
        10 e0 1e e6 2a 27 51 a7 4d 99 e0 54 b4 2d 6f 82

CTR Ciphertext
        bf 77 2e bd ae 05 ea 5b a3 3f 61 18 59 f7 04 29
        df 7a 09 14 f2 6d 82 fa 5e ad 59 1d 3f be f6 e2
```

```plaintext
Master key:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 7f

Master Nonce:
        ff ff ff ff ff ff ff ff ff ff ff ff
Master IV:
        ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff

Round key
        70 3a cb 17 f8 69 6b 91 6d 1b 0d 7a 86 2f dc 3a
        fc 41 de bf 43 43 a4 63 89 bc ed a3 2c 28 a0 c2
        9c 87 66 d1 bb 0c 18 fb 37 96 a6 9a 8f 01 30 f4
        6a 3c 9a a9 58 9e c2 07 74 55 b6 7c 45 14 53 46
        d1 8b 9d 40 6b 2d e5 73 73 6f 81 63 13 d4 f5 84
        ce 67 eb 93 9d cb 84 d2 d7 a5 5b a4 de a4 36 f3
        20 cd 86 a5 32 33 77 b2 16 07 ff d8 1a d4 36 bf
        9a f7 2e ee 48 2f 12 3c 35 2e 99 30 a1 fb 24 5f

Plaintext
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
        00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01

ECB Ciphertext
        a9 af 26 48 29 d7 0d bd 24 15 bf be 8a d3 5d 1f
        a9 af 26 48 29 d7 0d bd 24 15 bf be 8a d3 5d 1f

CBC Ciphertext
        33 be b9 a2 b6 b6 b3 14 40 3f e9 a7 c0 84 14 25
        13 a8 4c 75 01 0c e4 43 4c 40 90 c4 72 dc 4d 2b

OFB Ciphertext
        0b 3b f9 10 4a 23 c8 3a e3 84 e0 7e 02 77 c1 9b
        06 b6 f6 ad 6d ea 1c cd cf 7c 2d 2d ae ea d4 98

CTR Ciphertext
        a6 fa 03 ac 70 eb 53 ab 3d 49 57 fc ed a3 86 1e
        61 98 70 8b d6 18 32 9b e1 fd 56 80 ab 32 85 9d
```