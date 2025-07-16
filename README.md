# 🔐 密码算法仓库说明 | Cipher Algorithm Repository Overview

本仓库收录了多种加密算法，包括：**AES**、**ChaCha20**、**OP4**、**FEA** 和 **SSC**。这些算法在设计上均考虑了安全性，但需要注意的是，本仓库仅包含加密部分，并不包含完整的认证机制，例如 **AES-GCM** 或 **ChaCha20-Poly1305**。

This repository contains a collection of encryption algorithms, including **AES**, **ChaCha20**, **OP4**, **FEA**, and **SSC**. All these algorithms are designed with security in mind. However, the repository provides encryption implementations only — authentication schemes such as **AES-GCM** and **ChaCha20-Poly1305** are not included.

---

## 🛡️ 安全性说明 | About Security

你可以放心地使用本仓库中的任意一种主算法，它们能够为你的数据提供充分的保护。

- **AES** 是依据官方标准实现；
- **ChaCha20** 完全遵循 [RFC 8439](https://www.rfc-editor.org/rfc/rfc8439)；
- 然而由于它们均为纯软件实现，其**性能可能不尽如人意**，在高性能场景下可能存在瓶颈。

Any of the core algorithms in this repository can be safely used — they all offer solid data protection.

- **AES** follows official standards.
- **ChaCha20** complies fully with [RFC 8439](https://www.rfc-editor.org/rfc/rfc8439).
- That said, as these are purely software-based implementations, their **performance may not be optimal** for high-throughput or time-sensitive scenarios.

---

## 🚫 Research 目录算法 | Algorithms in the Research Directory

请务必注意，`Research` 目录中的算法属于早期探索性设计，**并不被认为具备足够的安全性**。

- 这些算法仅供研究学习使用；
- **请不要将它们用于生产环境或任何实际数据保护任务**。

Be advised that algorithms under the `Research` directory are experimental designs created in the early development phase (such as SN-Grotesque variants). **They are not considered sufficiently secure**.

- These are provided strictly for **educational and research purposes**.
- **Do not use them in production systems or to protect sensitive data**.
