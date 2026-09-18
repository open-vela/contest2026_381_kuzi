# AI 管家 — 板载守望（Board Sentinel）

## 一、作品简介

**板载守望** 是跑在 **GD32F470V-START** 上的嵌入式 AI Agent 演示。  
**不依赖任何外接传感器 / WiFi 模组 / 继电器**，只用板载：

- 用户 LED ×3  
- 用户按键  
- 串口 CLI（USART0）

Agent 能 **主动巡检** 空闲与堆状态，并 **执行** LED 告警/闪烁，同时提供可扩展 Skill。

### 核心亮点

| 亮点 | 说明 |
|------|------|
| **主动 + 执行** | 定时巡检；空闲超时主动闪 LED2；按键主动闪 LED1 |
| **板载零外设** | 插电即可演示，无需扩展板 |
| **3 个自定义 Skill** | `board-status` / `led-control` / `board-watch` |
| **CLI 对话** | NSH 下运行 `ai_butler`，自然语言式短命令 |

### 用户故事

> 板子上电后启动守望 Agent。你一段时间不操作，它主动发现「空闲过久」并闪灯提醒。  
> 你输入 `status` 查看堆与 uptime；输入 `led 1 blink` 立刻执行闪灯。按下用户键，它主动响应并闪 LED1。

---

## 二、选题方向

**AI 硬件产品创新赛道** — openvela 嵌入式 Agent：「能主动、会执行」，硬件仅限 START 板载资源。

---

## 三、目录结构

```
contest2026_381_kuzi/
├── app/ai_butler/
│   ├── src/
│   │   ├── ai_butler_main.c      # Agent 主循环 + CLI
│   │   ├── board_monitor.c/.h    # 板载状态 / LED / 按键
│   │   └── ai_butler.h
│   └── skills/
│       ├── board-status.md
│       ├── led-control.md
│       └── board-watch.md
├── board/contest_board/          # 板级配置
├── logs/                         # AI Coding 日志
├── docs/
└── README.md
```

---

## 四、运行方式

```bash
# 在 openvela 工作区根目录编译（路径按你的 repo 布局调整）
./build.sh contest2026_381_board:nsh -j8
# 或官方 START 板 NSH + 挂载 ai_butler 应用后编译

openocd -f interface/cmsis-dap.cfg -f target/stm32f4x.cfg \
  -c "program nuttx/nuttx.bin verify reset exit 0x08000000"
```

```text
nsh> ai_butler
ai_butler> help
ai_butler> status
ai_butler> led 1 blink
ai_butler> inspect
```

建议启用（defconfig）：`CONFIG_AI_BUTLER=y`，以及 `CONFIG_USERLED` / `CONFIG_INPUT_BUTTONS`（有则硬件灯/键，无则 soft 日志执行）。

### 演示场景（拍视频够用）

1. 启动 `ai_butler`，展示 Skill 列表  
2. 等待空闲告警 → LED 闪烁（主动+执行）  
3. `led 2 on` / `status`  
4. 按用户键 → 主动闪 LED1  

---

## 五、AI Coding

全程用 Cursor / Claude Code 辅助：需求收敛为板载-only、Skill 设计、实现与调试。日志见 `logs/`。

---

## 六、与旧方案差异

| 旧方案 | 现方案 |
|--------|--------|
| SHT30 / CO₂ / 继电器 / ESP32 / 语音 | **全部去掉** |
| 依赖外设才能演示 | **仅 START 板** |

评分相关能力保留：**主动巡检、阈值执行、自定义 Skill、板载可运行 Agent**。
