# 丁真时钟 — Machine Clock

STM32F103C8T6 智能机械时钟项目，30xSG90S 舵机 + 3D 打印色块模拟 4 位 7 段数码管。

## 项目路径

```
D:\AAAend\STM32\丁真时钟\
├── Machine_Clock/     ← Git 仓库 (本目录)
├── 电源板/            ← PCB 设计主目录 (v3 最新)
├── PCB文件/           ← 主板 Gerber + PCA9685 资料
├── 1~6 文件夹         ← 代码/语音/图纸/物料
```

## Git

- Remote: git@github.com:BEelzeBub5656/Machine_Clock.git
- SSH Key: C:\Users\73449\565627pp
- Git Bash: D:\Git\usr\bin\bash.exe

## 电源板 v3.0 (当前版本)

- 尺寸: 65×50mm, 2oz 双面板
- 输入: LRS-50-5 (5V/10A)
- 输出: 5V_SERVO(10A) / 5V_LOGIC / 5V_SPARE / 3.3V / 3.3V_SPARE
- 13 个元件 (J1,J2,KF128; L1,TOROID-16MM; C1-C5 电解+MLCC; U1,AMS1117; J3-J6,HDR-2.54)
- 4 个网络: 5V_RAW, 5V_FILT, 3.3V, GND
- Gerber 已导出到 电源板/gerber/
- PCB JSON 格式为立创EDA标准版 `head: "3~1.11.3~~"`

## 当前任务

**目标**: 在立创EDA Pro 里手动重建电源板原理图+PCB

**原因**: 
- gen_v3.py 生成的 JSON 是标准版格式，无法导入 Pro
- 标准版 lceda.cn 的 Open→JLCEDA 也无反应
- 13个元件，手动重画很快

**步骤**:
1. 打开 D:\lceda-pro\lceda-pro.exe
2. 新建工程 → 画原理图 (参考 D:\AAAend\STM32\丁真时钟\电源板\电源板_v3_原理图.svg)
3. 画 PCB (65x50mm, 2oz, 5mm舵机轨)
4. 导出 Gerber

## Skills 配置

在 C:\Users\73449\.claude\settings.json 启用了:
- superpowers, ui-ux-pro-max, document-skills, ralph-loop, frontend-design, karpathy-skills

## MCP 配置

在 C:\Users\73449\.claude.json 配置了:
- chrome-devtools, playwright, filesystem, firecrawl, cloud-hermes
