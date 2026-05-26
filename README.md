# Machine Clock (STM32 机械时钟)

基于 STM32F103C8T6 的智能机械时钟，30 个 SG90S 舵机 + 3D 打印色块模拟 4 位 7 段数码管。

## 硬件架构

```
220V AC → LRS开关电源(5V) → 电源板(LC滤波+LDO) → 三路输出
                                              ├── 5V_SERVO → PCA9685×2 → 30×SG90S舵机
                                              ├── 5V_LOGIC → ASRPRO语音模块 + PCA9685逻辑
                                              └── 3.3V → STM32F103C8T6
```

## 目录

```
Machine_Clock/
├── README.md                          ← 本文件
├── 电源板/                             ← 电源管理PCB设计
│   ├── gen_v3.py                      ← v3生成脚本 (Python)
│   ├── BOM_接线指南.md                 ← v2 BOM
│   ├── BOM_接线指南_v3.md              ← v3 BOM
│   ├── 电源板原理图.svg                ← v2 原理图预览
│   ├── 电源板_v3_原理图.svg            ← v3 原理图预览
│   ├── 机械时钟电源板_原理图.json      ← v2 原理图
│   ├── 机械时钟电源板_PCB.json         ← v2 PCB
│   ├── 机械时钟电源板_完整工程.zip     ← v2 完整工程
│   ├── 机械时钟电源板_v3_原理图.json   ← v3 原理图
│   ├── 机械时钟电源板_v3_PCB.json      ← v3 PCB
│   ├── 机械时钟电源板_v3_完整工程.zip  ← v3 完整工程
│   └── gerber/                         ← v3 Gerber文件
│       ├── PowerBoard_v3.GTL
│       ├── PowerBoard_v3.GTS
│       ├── PowerBoard_v3.GTO
│       ├── PowerBoard_v3.GBL
│       ├── PowerBoard_v3.GBS
│       ├── PowerBoard_v3.GKO
│       └── PowerBoard_v3.TXT
├── PCB文件/                            ← 主板Gerber文件
│   └── Gerber_基于stm32的智能机械时钟PCB_2025-10-13.zip
└── 代码/                               ← STM32固件 (待提交)
```

## 电源板 v3.0 (当前版本)

| 项目 | 规格 |
|------|------|
| 尺寸 | 65×50mm 双面板 2oz |
| 输入 | LRS-50-5 (5V/10A) |
| 输出 | 5V_SERVO (10A) / 5V_LOGIC / 5V_SPARE / 3.3V / 3.3V_SPARE |
| 滤波 | 10μH/10A 环形电感 + 470μF/16V (LC低通) |
| 稳压 | AMS1117-3.3V 模块 (3脚排母) |
| 接地 | 底层整面铜箔星型一点接地 |
| 舵机轨 | 5mm宽走线 (≥10A @ 2oz) |

### v3 改进

- 板面积从 45×38mm 增大到 65×50mm，提升散热
- 铜厚从 1oz 升级到 2oz
- 新增 J5 (5V_SPARE) 和 J6 (3.3V_SPARE) 扩展接口
- 舵机轨加宽至 5mm，支持 30×SG90 满载 10A
- AMS1117 输入/输出各加 0.1uF MLCC 去耦 (C4, C5)
- 生成了标准 Gerber 直接可投板

### BOM (13 元件)

| 元件 | 规格 | 封装 | 数量 |
|------|------|------|------|
| 功率电感 L1 | 10μH/10A | TOROID-16MM | 1 |
| 电解电容 C1 | 470μF/16V | DIP-10×16mm | 1 |
| 电解电容 C2 | 100μF/16V | DIP-6.3×11mm | 1 |
| 电解电容 C3 | 10μF/10V | DIP-6.3×11mm | 1 |
| MLCC C4, C5 | 0.1μF (104) | 0805 | 2 |
| 排母 U1 | 1×3P 2.54mm | HDR-2.54-3P-F | 1 |
| AMS1117-3.3V模块 | 25×11mm | 直插 | 1 |
| 接线端子 J1, J2 | KF128-2P | 5.08mm间距 | 2 |
| 排针 J3, J4, J5, J6 | 1×2P 2.54mm | HDR-2.54-2P | 4 |

## 硬件清单

- STM32F103C8T6 主控
- PCA9685 ×2 (I2C地址: 0x40, 0x41)
- SG90S 舵机 ×30 (4位7段数码管模拟)
- DS1302 实时时钟模块
- ASRPRO 2.0 语音模块
- LRS-50-5 开关电源 (220V→5V, 10A)

## 防干扰设计

舵机翻转时 30 个 SG90S 产生 ≥3A 电流尖峰，通过 LC 滤波（10μH 电感 + 470μF 电解电容）隔离在舵机电源轨。STM32 和 PCA9685 逻辑侧从电感前取电，经 AMS1117 稳压后供电，不受浪涌影响。

## 已知问题

- I2C 上拉电阻：PCA9685 模块未集成，需在主板 SDA/SCL 各飞 4.7KΩ 到 3.3V
- 时钟暂停冻结：由电源噪声导致，电源板 v3.0 2oz+加宽走线应根治
- 时间漂移：待排查 DS1302 读取逻辑

## Gerber 投板

v3 Gerber 文件位于 `电源板/gerber/`，可直接上传 JLC 打样：

| 文件 | 层 |
|------|-----|
| PowerBoard_v3.GTL | 顶层线路 |
| PowerBoard_v3.GBL | 底层线路 |
| PowerBoard_v3.GTS | 顶层阻焊 |
| PowerBoard_v3.GBS | 底层阻焊 |
| PowerBoard_v3.GTO | 顶层丝印 |
| PowerBoard_v3.GKO | 外形层 |
| PowerBoard_v3.TXT | 钻孔文件 |
