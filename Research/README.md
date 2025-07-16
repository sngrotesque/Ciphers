# 密码学研究 / Cryptography Research

⚠ **警告：以下加密算法仅用于研究目的，严禁在生产环境中使用！**  
⚠ **警告：以下加密算法仅用于研究目的，严禁在生产环境中使用！**  
⚠ **警告：以下加密算法仅用于研究目的，严禁在生产环境中使用！**  

⚠ **WARNING: These encryption algorithms are for research purposes only. DO NOT use them in production environments!**  
⚠ **WARNING: These encryption algorithms are for research purposes only. DO NOT use them in production environments!**  
⚠ **WARNING: These encryption algorithms are for research purposes only. DO NOT use them in production environments!**  

**这些算法并不安全！严禁用于生产环境！**  
**这些算法并不安全！严禁用于生产环境！**  
**这些算法并不安全！严禁用于生产环境！**  

**These algorithms are NOT secure! DO NOT use them in production!**  
**These algorithms are NOT secure! DO NOT use them in production!**  
**These algorithms are NOT secure! DO NOT use them in production!**  

---

### Cipher1  
> 一个具有**故意设计漏洞**的测试型流密码  
> **存在风险**：  
> - 选择明文攻击（CPA）  
> - 已知明文攻击（KPA）  
> - 完整密钥流恢复  

### Cipher1  
> A test stream cipher with **intentional vulnerabilities**.  
> **Vulnerable to**:  
> - Chosen-plaintext attacks (CPA)  
> - Known-plaintext attacks (KPA)  
> - Full keystream recovery  

---

### FEA (Old)  
> 一个**未被充分测试**的块密码算法  
> **存在风险**：  
> - 对于某些变换来说线性程度过高  
> - 轮密钥逆向恢复可能性（未验证）

### FEA (Old)  
> A **not sufficiently tested** block cipher algorithm  
> **Potential risks**:  
> - Excessive linearity in certain transformations  
> - Possible recovery of round keys through reverse engineering *(unverified)*  

---

### SSE（SN流加密）  
> 一个**非标准**实现的流密码，存在严重缺陷  
> **存在风险**：  
> - CPA/KPA攻击  
> - 完整密钥流恢复（可获取过去/未来所有状态）  
> **缺陷**：密钥调度薄弱，从单个被破解状态可推导出所有密钥流状态  

### SSE (SN Stream Encryption)  
> A **non-standard** stream cipher implementation with critical flaws.  
> **Vulnerable to**:  
> - CPA/KPA attacks  
> - Complete keystream recovery (past/future states)  
> **Flaw**: Weak key scheduling allows deriving all keystream states from a single compromised state.  

---

### SDSE（SN数据流加密）  
> 一个**非标准**实现的流密码，存在严重缺陷  
> **存在风险**：  
> - CPA/KPA攻击  
> - 密钥流可逆向推导出原始密钥  
> **缺陷**：状态生成过程非线性不足，可通过代数攻击恢复密钥  

### SDSE (SN Data Stream Encryption)  
> A **non-standard** stream cipher implementation with critical flaws.  
> **Vulnerable to**:  
> - CPA/KPA attacks  
> - Keystream reversal to original key  
> **Flaw**: Insufficient nonlinearity in state generation enables key recovery from keystream states.  

---

### 主要风险  
1. **密钥流可逆性**：所有变体都允许攻击者：  
   - 从部分密钥流恢复初始密钥  
   - 一旦某个状态被破解，可解密所有过去/未来的消息  
2. **仅限学术研究**：这些算法仅用于演示密码学弱点，不能提供实际保护  

### Key Risks
1. **Keystream Reversibility**: All variants allow attackers to:  
   - Recover the initial key from partial keystreams.  
   - Decrypt all past/future messages once any state is compromised.  
2. **Academic Use Only**: Designed to demonstrate cryptographic weaknesses, not for real-world protection.  

### 为什么不安全？  
| 算法      | 关键缺陷                  | 实际攻击影响                  |  
|-----------|--------------------------|-----------------------------|  
| Cipher1   | 无密钥混合                | 可轻松恢复密钥流              |  
| SSE       | 线性状态转换              | 单个状态泄露导致完全破解        |  
| SDSE      | 非线性操作不足            | 可通过代数攻击恢复密钥          |  

**实际应用推荐**使用标准算法如**AES-GCM**或**ChaCha20-Poly1305**

### Why These Are Unsafe?  
| Algorithm | Critical Flaw | Practical Attack Impact |  
|-----------|--------------|-------------------------|  
| Cipher1   | No key mixing | Trivial keystream recovery |  
| SSE       | Linear state transition | Single state → full compromise |  
| SDSE      | Weak nonlinear ops | Key recovery via algebraic attacks |  

**For real-world applications**, use standardized algorithms like **AES-GCM** or **ChaCha20-Poly1305**.
