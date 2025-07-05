# 通用加密算法的 C++ 实现 / C++ Implementations of General-Purpose Cryptographic Algorithms

本仓库收录了一些通用加密算法的 **C++** 实现，均为纯 **C++** 编写。  
This repository contains implementations of several general-purpose cryptographic algorithms, all written in pure C++.

请注意：**FEA** 和 **SSC** 是我自主设计的对称加密算法。其中，**FEA** 属于分组密码，而 **SSC** 是流密码。  
Note: **FEA** and **SSC** are symmetric encryption algorithms of my own design. **FEA** is a block cipher, while **SSC** is a stream cipher.

尽管它们尚未广泛流行，但在设计上已具备充分的安全性。  
Although not widely adopted, both algorithms are designed to offer robust security.

**OP4** 将作为最终的自研加密算法使用，需设计为支持 **SIMD** 且保持高安全性。  
**OP4** will be used as the final self-developed encryption algorithm, designed to support **SIMD** while maintaining high security.

此外，仓库还实现了以下标准算法：**AES**（支持 **128/192/256** 位密钥）和 **ChaCha20**（完全符合 **RFC 8439** 标准）。  
In addition, the repository includes implementations of standard algorithms such as **AES** (supporting **128/192/256**-bit keys) and **ChaCha20** (fully compliant with **RFC 8439**).

⚠️ 警告：本仓库**未实现任何用于消息完整性验证的算法**（如 **AES-GCM** 或 **ChaCha20-Poly1305**），仅包含加密算法本身。  
⚠️ Warning: This repository does **not implement any algorithms for message integrity verification** (e.g., **AES-GCM** or **ChaCha20-Poly1305**); it only includes the encryption algorithms themselves.
