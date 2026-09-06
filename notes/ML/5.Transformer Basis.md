## 导论：序列建模的范式革命

Transformer 是由 Vaswani 等人在 2017 年论文《Attention Is All You Need》中提出的架构，它彻底抛弃了 RNN 的循环递归结构与 CNN 的局部感受野，首次完全基于**注意力机制（Attention Mechanism）** 来建模序列间的长程依赖。它不仅是 NLP 领域的分水岭，更是 GPT、BERT、LLaMA 等所有现代大语言模型的基石。

Transformer 的核心哲学在于：**将序列建模转化为集合运算**——通过一次性的两两交互（点积注意力），让每个位置直接获取全局上下文，从而从根本上消除了信息传递的“瓶颈”并释放了并行计算的巨大潜力。

---

## 第一部分：缩放点积注意力（Scaled Dot-Product Attention）

注意力机制的本质是**基于查询（Query）与键（Key）的相似度，对值（Value）进行加权聚合**。

### 1.1 严格定义
给定查询矩阵 \( \mathbf{Q} \in \mathbb{R}^{n \times d_k} \)、键矩阵 \( \mathbf{K} \in \mathbb{R}^{m \times d_k} \) 和值矩阵 \( \mathbf{V} \in \mathbb{R}^{m \times d_v} \)，注意力输出为：
\[
\text{Attention}(\mathbf{Q}, \mathbf{K}, \mathbf{V}) = \text{softmax}\left( \frac{\mathbf{Q} \mathbf{K}^T}{\sqrt{d_k}} \right) \mathbf{V}
\]

**核心组件拆解**：

- **得分矩阵**：\( \mathbf{S} = \mathbf{Q} \mathbf{K}^T \in \mathbb{R}^{n \times m} \)，其中第 \( i \) 行第 \( j \) 列元素为 \( \mathbf{q}_i \cdot \mathbf{k}_j \)，度量第 \( i \) 个查询与第 \( j \) 个键的匹配程度（未归一化的内积）。
- **缩放因子**：\( \sqrt{d_k} \)。由于内积的大小随维度 \( d_k \) 增长（向量各元素独立同分布时，内积方差为 \( d_k \)），若不缩放，得分绝对值过大，导致 softmax 函数的梯度集中在极其饱和的区域（导数趋近于 0），造成梯度消失。除以 \( \sqrt{d_k} \) 使得分方差稳定在 1，确保梯度流动顺畅。
- **Softmax 归一化**：按行执行 softmax，使每个位置的权重和为 1，即：
  \[
  \alpha_{ij} = \frac{\exp(\mathbf{q}_i \cdot \mathbf{k}_j / \sqrt{d_k})}{\sum_{l=1}^{m} \exp(\mathbf{q}_i \cdot \mathbf{k}_l / \sqrt{d_k})}
  \]
- **加权求和**：输出矩阵的第 \( i \) 行为 \( \sum_j \alpha_{ij} \mathbf{v}_j \)，即对 \( \mathbf{V} \) 的行进行凸组合。

---

## 第二部分：多头注意力（Multi-Head Attention, MHA）

单一注意力头只能捕获一种特征子空间内的依赖模式。多头注意力通过将查询、键、值投影到多个低维子空间，并行执行多次注意力计算，使模型能够从不同角度（如语法、语义、指代）同时关注信息。

### 2.1 严格数学定义
\[
\text{MultiHead}(\mathbf{Q}, \mathbf{K}, \mathbf{V}) = \text{Concat}(\text{head}_1, \dots, \text{head}_h) \mathbf{W}^O
\]
其中每个头为：
\[
\text{head}_i = \text{Attention}(\mathbf{Q} \mathbf{W}_i^Q, \mathbf{K} \mathbf{W}_i^K, \mathbf{V} \mathbf{W}_i^V)
\]

- **投影矩阵**：\( \mathbf{W}_i^Q \in \mathbb{R}^{d_{\text{model}} \times d_k} \)，\( \mathbf{W}_i^K \in \mathbb{R}^{d_{\text{model}} \times d_k} \)，\( \mathbf{W}_i^V \in \mathbb{R}^{d_{\text{model}} \times d_v} \)，\( \mathbf{W}^O \in \mathbb{R}^{h d_v \times d_{\text{model}}} \)。
- 通常取 \( d_k = d_v = d_{\text{model}} / h \)，即总参数量不随头数增加而爆炸（保持计算量恒定）。

> **深刻理解**：多头机制允许每个头关注序列中不同的“关系类型”。例如在 Transformer 中，某些头专注于邻近词（局部句法），某些头关注远距离指代（全局语义）。这本质上是**在不增加网络深度的前提下，横向扩展了模型的表征容量**。

---

## 第三部分：位置编码（Positional Encoding）

自注意力是**排列不变（Permutation Invariant）** 的——打乱输入序列顺序，注意力输出仅相应打乱，但不改变计算出的数值关系。然而序列的**顺序**蕴含着至关重要的信息（如“狗咬人”与“人咬狗”）。

### 3.1 绝对位置编码（原始 Transformer 方案）
对位置 \( pos \) 和维度 \( i \) 使用正弦与余弦函数：
\[
PE_{(pos, 2i)} = \sin\left( \frac{pos}{10000^{2i / d_{\text{model}}}} \right)
\]
\[
PE_{(pos, 2i+1)} = \cos\left( \frac{pos}{10000^{2i / d_{\text{model}}}} \right)
\]
位置编码与词嵌入直接相加（而非拼接）：\( \mathbf{x}_{pos} = \mathbf{x}_{pos} + PE_{pos} \)。

> **数学性质**：此编码方案允许模型通过三角恒等式轻松学习相对位置关系（即 \( PE_{pos+k} \) 可表示为 \( PE_{pos} \) 的线性变换），同时各维度频率不同，使模型能区分远近不同的位置。

### 3.2 后续变体（RoPE 与 ALiBi）
现代大模型（如 LLaMA）普遍采用**旋转位置编码（Rotary Position Embedding, RoPE）** 或将位置偏置直接注入注意力分数中（ALiBi），因为它们能更好地处理超长上下文（超过训练长度）的外推问题。

---

## 第四部分：完整架构块（编码器与解码器）

### 4.1 子层结构（Sub-layer Block）
每个子层（Sub-layer）都遵循 **“残差连接 + 层归一化”** 的范式（Pre-LN 结构为主流）：
\[
\mathbf{x}_{\text{out}} = \text{LayerNorm}(\mathbf{x}_{\text{in}} + \text{SubLayer}(\mathbf{x}_{\text{in}}))
\]

现代实现（如 GPT）更常用 **Pre-LayerNorm**（先归一化，再进入子层）：
\[
\mathbf{x}_{\text{out}} = \mathbf{x}_{\text{in}} + \text{SubLayer}(\text{LayerNorm}(\mathbf{x}_{\text{in}}))
\]
Pre-LN 结构显著缓解了深度堆叠时的梯度消失，无需精细调整 warmup 步骤。

### 4.2 前馈网络（Position-wise Feed-Forward Network, FFN）
FFN 是一个作用于**每个位置独立且相同**的两层 MLP：
\[
\text{FFN}(\mathbf{x}) = \text{GeLU}(\mathbf{x} \mathbf{W}_1 + \mathbf{b}_1) \mathbf{W}_2 + \mathbf{b}_2
\]
（原始论文使用 ReLU，现代常用 GeLU / Swish）。FFN 承担了模型的大部分参数（约 2/3），负责将注意力提取的交互特征进行非线性变换和维度扩展（通常内层维度 \( d_{ff} = 4 \times d_{\text{model}} \)）。

### 4.3 编码器（Encoder）与解码器（Decoder）的区别
- **编码器（如 BERT）**：由 \( N \) 个相同的块堆叠，每个块包含 **多头自注意力**（双向，可看到所有 token）和 **FFN**。用于提取上下文表征。
- **解码器（如 GPT）**：由 \( N \) 个块堆叠，每个块包含：
  1. **掩码多头自注意力**（Causal Attention，只能看到左侧 token，防止窥视未来）。
  2. **交叉注意力**（Cross-Attention，仅在原始 Encoder-Decoder 模型中存在）：查询来自解码器上一层，键和值来自编码器最后一层输出。用于机器翻译等序列生成任务。
  3. **FFN**。
  现代自回归大语言模型（LLM）大多舍弃了交叉注意力，仅使用 **解码器架构（Decoder-only）**，通过因果掩码实现自回归生成。

---

## 第五部分：掩码机制（Masking）

- **填充掩码（Padding Mask）**：将输入中填充的无效位置（如 [PAD] token）在 softmax 之前赋予 \( -\infty \)，使注意力权重趋近于 0。
- **因果掩码（Causal / Future Mask）**：在自回归解码时，构造一个上三角矩阵（对于第 \( i \) 个查询，禁止其关注第 \( j > i \) 个键），将对应得分设为 \( -\infty \)。数学表达为：
  \[
  \text{Mask}_{ij} = \begin{cases} 0 & j \leq i \\ -\infty & j > i \end{cases}
  \]

---

## 第六部分：计算复杂度与归纳偏置

### 6.1 计算复杂度
对于序列长度 \( n \)：
- **自注意力**：\( O(n^2 \cdot d) \)（\( n^2 \) 来自 \( \mathbf{Q}\mathbf{K}^T \) 的矩阵乘法）。这是 Transformer 最大的瓶颈（上下文长度二次增长）。
- **FFN**：\( O(n \cdot d^2) \)（与位置数量线性相关）。

> **对比**：RNN 复杂度为 \( O(n \cdot d^2) \)，但无法并行且存在遗忘门的信息损失；Transformer 以 \( O(n^2) \) 的代价换取了完美的长程依赖捕获和全并行训练。

### 6.2 归纳偏置（Inductive Bias）
- **RNN/CNN**：带有强烈的局部性和顺序先验（时间或空间连续性）。
- **Transformer**：**极弱的归纳偏置**。它几乎完全依赖数据本身来学习结构（通过注意力权重）。这使得 Transformer 需要海量数据才能学会语法和逻辑，但一旦数据量足够，其灵活性远超强先验的模型，这正是 Scaling Law 成立的结构性原因。

---

## 结语：注意力即一切

Transformer 不仅是一个架构，更是一种**将关系建模转化为可微矩阵乘法**的工程哲学。它的成功揭示了三个本质：

1. **并行性压倒串行性**：以计算量换时间步，使超大规模分布式训练成为可能。
2. **全局感受野是涌现智能的温床**：每个 token 直接面对所有 token，为上下文学习和思维链（CoT）提供了物理空间。
3. **简单结构 + 海量数据 > 复杂先验**：Transformer 证明了足够灵活的通用计算引擎可以在足够大的语料上“吞噬”掉人工构造的偏见。

理解 Transformer，就是理解现代人工通用智能（AGI）路线图的底层数学骨架——从矩阵乘法到注意力权重，再到残差链上的梯度流，每一行代码背后都是严谨的线性代数与概率统计的胜利。## 导论：序列建模的范式革命

Transformer 是由 Vaswani 等人在 2017 年论文《Attention Is All You Need》中提出的架构，它彻底抛弃了 RNN 的循环递归结构与 CNN 的局部感受野，首次完全基于**注意力机制（Attention Mechanism）** 来建模序列间的长程依赖。它不仅是 NLP 领域的分水岭，更是 GPT、BERT、LLaMA 等所有现代大语言模型的基石。

Transformer 的核心哲学在于：**将序列建模转化为集合运算**——通过一次性的两两交互（点积注意力），让每个位置直接获取全局上下文，从而从根本上消除了信息传递的“瓶颈”并释放了并行计算的巨大潜力。

---

## 第一部分：缩放点积注意力（Scaled Dot-Product Attention）

注意力机制的本质是**基于查询（Query）与键（Key）的相似度，对值（Value）进行加权聚合**。

### 1.1 严格定义
给定查询矩阵 \( \mathbf{Q} \in \mathbb{R}^{n \times d_k} \)、键矩阵 \( \mathbf{K} \in \mathbb{R}^{m \times d_k} \) 和值矩阵 \( \mathbf{V} \in \mathbb{R}^{m \times d_v} \)，注意力输出为：
\[
\text{Attention}(\mathbf{Q}, \mathbf{K}, \mathbf{V}) = \text{softmax}\left( \frac{\mathbf{Q} \mathbf{K}^T}{\sqrt{d_k}} \right) \mathbf{V}
\]

**核心组件拆解**：

- **得分矩阵**：\( \mathbf{S} = \mathbf{Q} \mathbf{K}^T \in \mathbb{R}^{n \times m} \)，其中第 \( i \) 行第 \( j \) 列元素为 \( \mathbf{q}_i \cdot \mathbf{k}_j \)，度量第 \( i \) 个查询与第 \( j \) 个键的匹配程度（未归一化的内积）。
- **缩放因子**：\( \sqrt{d_k} \)。由于内积的大小随维度 \( d_k \) 增长（向量各元素独立同分布时，内积方差为 \( d_k \)），若不缩放，得分绝对值过大，导致 softmax 函数的梯度集中在极其饱和的区域（导数趋近于 0），造成梯度消失。除以 \( \sqrt{d_k} \) 使得分方差稳定在 1，确保梯度流动顺畅。
- **Softmax 归一化**：按行执行 softmax，使每个位置的权重和为 1，即：
  \[
  \alpha_{ij} = \frac{\exp(\mathbf{q}_i \cdot \mathbf{k}_j / \sqrt{d_k})}{\sum_{l=1}^{m} \exp(\mathbf{q}_i \cdot \mathbf{k}_l / \sqrt{d_k})}
  \]
- **加权求和**：输出矩阵的第 \( i \) 行为 \( \sum_j \alpha_{ij} \mathbf{v}_j \)，即对 \( \mathbf{V} \) 的行进行凸组合。

---

## 第二部分：多头注意力（Multi-Head Attention, MHA）

单一注意力头只能捕获一种特征子空间内的依赖模式。多头注意力通过将查询、键、值投影到多个低维子空间，并行执行多次注意力计算，使模型能够从不同角度（如语法、语义、指代）同时关注信息。

### 2.1 严格数学定义
\[
\text{MultiHead}(\mathbf{Q}, \mathbf{K}, \mathbf{V}) = \text{Concat}(\text{head}_1, \dots, \text{head}_h) \mathbf{W}^O
\]
其中每个头为：
\[
\text{head}_i = \text{Attention}(\mathbf{Q} \mathbf{W}_i^Q, \mathbf{K} \mathbf{W}_i^K, \mathbf{V} \mathbf{W}_i^V)
\]

- **投影矩阵**：\( \mathbf{W}_i^Q \in \mathbb{R}^{d_{\text{model}} \times d_k} \)，\( \mathbf{W}_i^K \in \mathbb{R}^{d_{\text{model}} \times d_k} \)，\( \mathbf{W}_i^V \in \mathbb{R}^{d_{\text{model}} \times d_v} \)，\( \mathbf{W}^O \in \mathbb{R}^{h d_v \times d_{\text{model}}} \)。
- 通常取 \( d_k = d_v = d_{\text{model}} / h \)，即总参数量不随头数增加而爆炸（保持计算量恒定）。

> **深刻理解**：多头机制允许每个头关注序列中不同的“关系类型”。例如在 Transformer 中，某些头专注于邻近词（局部句法），某些头关注远距离指代（全局语义）。这本质上是**在不增加网络深度的前提下，横向扩展了模型的表征容量**。

---

## 第三部分：位置编码（Positional Encoding）

自注意力是**排列不变（Permutation Invariant）** 的——打乱输入序列顺序，注意力输出仅相应打乱，但不改变计算出的数值关系。然而序列的**顺序**蕴含着至关重要的信息（如“狗咬人”与“人咬狗”）。

### 3.1 绝对位置编码（原始 Transformer 方案）
对位置 \( pos \) 和维度 \( i \) 使用正弦与余弦函数：
\[
PE_{(pos, 2i)} = \sin\left( \frac{pos}{10000^{2i / d_{\text{model}}}} \right)
\]
\[
PE_{(pos, 2i+1)} = \cos\left( \frac{pos}{10000^{2i / d_{\text{model}}}} \right)
\]
位置编码与词嵌入直接相加（而非拼接）：\( \mathbf{x}_{pos} = \mathbf{x}_{pos} + PE_{pos} \)。

> **数学性质**：此编码方案允许模型通过三角恒等式轻松学习相对位置关系（即 \( PE_{pos+k} \) 可表示为 \( PE_{pos} \) 的线性变换），同时各维度频率不同，使模型能区分远近不同的位置。

### 3.2 后续变体（RoPE 与 ALiBi）
现代大模型（如 LLaMA）普遍采用**旋转位置编码（Rotary Position Embedding, RoPE）** 或将位置偏置直接注入注意力分数中（ALiBi），因为它们能更好地处理超长上下文（超过训练长度）的外推问题。

---

## 第四部分：完整架构块（编码器与解码器）

### 4.1 子层结构（Sub-layer Block）
每个子层（Sub-layer）都遵循 **“残差连接 + 层归一化”** 的范式（Pre-LN 结构为主流）：
\[
\mathbf{x}_{\text{out}} = \text{LayerNorm}(\mathbf{x}_{\text{in}} + \text{SubLayer}(\mathbf{x}_{\text{in}}))
\]

现代实现（如 GPT）更常用 **Pre-LayerNorm**（先归一化，再进入子层）：
\[
\mathbf{x}_{\text{out}} = \mathbf{x}_{\text{in}} + \text{SubLayer}(\text{LayerNorm}(\mathbf{x}_{\text{in}}))
\]
Pre-LN 结构显著缓解了深度堆叠时的梯度消失，无需精细调整 warmup 步骤。

### 4.2 前馈网络（Position-wise Feed-Forward Network, FFN）
FFN 是一个作用于**每个位置独立且相同**的两层 MLP：
\[
\text{FFN}(\mathbf{x}) = \text{GeLU}(\mathbf{x} \mathbf{W}_1 + \mathbf{b}_1) \mathbf{W}_2 + \mathbf{b}_2
\]
（原始论文使用 ReLU，现代常用 GeLU / Swish）。FFN 承担了模型的大部分参数（约 2/3），负责将注意力提取的交互特征进行非线性变换和维度扩展（通常内层维度 \( d_{ff} = 4 \times d_{\text{model}} \)）。

### 4.3 编码器（Encoder）与解码器（Decoder）的区别
- **编码器（如 BERT）**：由 \( N \) 个相同的块堆叠，每个块包含 **多头自注意力**（双向，可看到所有 token）和 **FFN**。用于提取上下文表征。
- **解码器（如 GPT）**：由 \( N \) 个块堆叠，每个块包含：
  1. **掩码多头自注意力**（Causal Attention，只能看到左侧 token，防止窥视未来）。
  2. **交叉注意力**（Cross-Attention，仅在原始 Encoder-Decoder 模型中存在）：查询来自解码器上一层，键和值来自编码器最后一层输出。用于机器翻译等序列生成任务。
  3. **FFN**。
  现代自回归大语言模型（LLM）大多舍弃了交叉注意力，仅使用 **解码器架构（Decoder-only）**，通过因果掩码实现自回归生成。

---

## 第五部分：掩码机制（Masking）

- **填充掩码（Padding Mask）**：将输入中填充的无效位置（如 [PAD] token）在 softmax 之前赋予 \( -\infty \)，使注意力权重趋近于 0。
- **因果掩码（Causal / Future Mask）**：在自回归解码时，构造一个上三角矩阵（对于第 \( i \) 个查询，禁止其关注第 \( j > i \) 个键），将对应得分设为 \( -\infty \)。数学表达为：
  \[
  \text{Mask}_{ij} = \begin{cases} 0 & j \leq i \\ -\infty & j > i \end{cases}
  \]

---

## 第六部分：计算复杂度与归纳偏置

### 6.1 计算复杂度
对于序列长度 \( n \)：
- **自注意力**：\( O(n^2 \cdot d) \)（\( n^2 \) 来自 \( \mathbf{Q}\mathbf{K}^T \) 的矩阵乘法）。这是 Transformer 最大的瓶颈（上下文长度二次增长）。
- **FFN**：\( O(n \cdot d^2) \)（与位置数量线性相关）。

> **对比**：RNN 复杂度为 \( O(n \cdot d^2) \)，但无法并行且存在遗忘门的信息损失；Transformer 以 \( O(n^2) \) 的代价换取了完美的长程依赖捕获和全并行训练。

### 6.2 归纳偏置（Inductive Bias）
- **RNN/CNN**：带有强烈的局部性和顺序先验（时间或空间连续性）。
- **Transformer**：**极弱的归纳偏置**。它几乎完全依赖数据本身来学习结构（通过注意力权重）。这使得 Transformer 需要海量数据才能学会语法和逻辑，但一旦数据量足够，其灵活性远超强先验的模型，这正是 Scaling Law 成立的结构性原因。

---

## 结语：注意力即一切

Transformer 不仅是一个架构，更是一种**将关系建模转化为可微矩阵乘法**的工程哲学。它的成功揭示了三个本质：

1. **并行性压倒串行性**：以计算量换时间步，使超大规模分布式训练成为可能。
2. **全局感受野是涌现智能的温床**：每个 token 直接面对所有 token，为上下文学习和思维链（CoT）提供了物理空间。
3. **简单结构 + 海量数据 > 复杂先验**：Transformer 证明了足够灵活的通用计算引擎可以在足够大的语料上“吞噬”掉人工构造的偏见。

理解 Transformer，就是理解现代人工通用智能（AGI）路线图的底层数学骨架——从矩阵乘法到注意力权重，再到残差链上的梯度流，每一行代码背后都是严谨的线性代数与概率统计的胜利。