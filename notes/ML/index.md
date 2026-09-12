# Machine Learning 笔记

大部分AI生成

## 学习过程
#

#

```mermaid
flowchart TD
    %% 样式
    classDef meta fill:#f4f6f7,stroke:#7f8c8d,stroke-width:1px;
    classDef math fill:#f9f0ff,stroke:#9b59b6,stroke-width:2px;
    classDef data fill:#e8f8f5,stroke:#1abc9c,stroke-width:2px;
    classDef classic fill:#fef9e7,stroke:#f1c40f,stroke-width:2px;
    classDef dl fill:#ebf5fb,stroke:#3498db,stroke-width:2px;
    classDef advanced fill:#fdedec,stroke:#e74c3c,stroke-width:2px;

    %% 阶段0
    Phil(("📜 阶段0：历史与哲学<br>（认知起点）")):::meta

    %% 阶段1
    subgraph L1 [阶段1：数理支柱]
        LA[🧮 线性代数<br>（SVD/伪逆）]:::math
        Calc[📈 微积分与∇算子<br>（链式法则）]:::math
        Prob[🎲 概率与信息论<br>（熵/KL散度）]:::math
    end

    %% 阶段2
    subgraph L2 [阶段2：数据与降维]
        DP[🧹 数据处理与增强]:::data
        DR[🔭 降维可视化<br>（PCA/t-SNE）]:::data
    end

    %% 阶段3
    subgraph L3 [阶段3：经典集成]
        Tree[🌲 集成树<br>（XGBoost/LGBM）]:::classic
        Metrics[📊 离线指标矩阵<br>（AUC/鲁棒性）]:::classic
    end

    %% 阶段4
    subgraph L4 [阶段4：深度引擎]
        MLP[🧠 MLP与反向传播<br>（BP推导）]:::dl
        NonLin[⚡ 非线性与归一化<br>（BN/LN/ReLU）]:::dl
        Opt[🎯 优化器与调度<br>（AdamW/动量）]:::dl
    end

    %% 阶段5
    subgraph L5 [阶段5：感知架构]
        RNN[🔄 循环网络<br>（LSTM/GRU）]:::advanced
        Trans[🧩 Transformer<br>（注意力/RoPE）]:::advanced
        Vision[🖼️ 计算机视觉<br>（ViT/Swin）]:::advanced
    end

    %% 阶段6
    RL[🤖 阶段6：强化学习<br>（PPO/Alpha Zero）]:::advanced

    %% ============ 依赖连线 ============
    Phil -.-> L1
    Phil -.-> L3

    LA --> MLP
    Calc --> MLP
    Prob --> Tree
    Prob --> Trans

    DP --> MLP
    DP --> Tree

    MLP --> NonLin
    MLP & NonLin --> Opt
    MLP & Opt --> RNN
    MLP & Opt --> Trans

    Tree --> Metrics
    MLP --> Metrics

    RNN -.-> Trans
    MLP & Trans --> Vision

    MLP & Opt & Prob --> RL
```
## 常用网站

### 实践练习
#

- kaggle

- 晨涧云 租用cuda

- huggingface  (for dataset, model)  
- bohrium (慎用)

- minimind github
### 技术文档

- pytorch

- sklearn

- [https://transformers.run/](https://transformers.run/)