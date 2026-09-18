# AI 管家 - 用户手册

## 1. 产品简介

AI 管家是基于 openvela + ai_agent 框架开发的智能环境管家系统，运行在 GD32F470V-START 开发板上。系统集成了环境感知、语音交互、设备控制、场景联动等功能，为您提供智能化的家居管理体验。

### 1.1 核心功能

- **连续对话**: 支持多轮语音对话，上下文理解
- **模糊意图理解**: 无需精确指令，理解自然语言
- **主动提醒**: 环境异常告警、定时提醒、健康提醒
- **场景联动**: 自动化场景控制，智能联动

### 1.2 技术特点

- 基于 openvela 实时操作系统
- 使用 ai_agent 框架，支持 Skills 扩展
- 集成火山引擎 ASR/TTS
- 支持 DeepSeek LLM 大模型
- 模块化设计，易于扩展

## 2. 硬件连接

### 2.1 开发板信息

- **型号**: GD32F470V-START
- **MCU**: Arm Cortex-M4F @ 240 MHz
- **内存**: 256KB SRAM + 64KB TCM
- **Flash**: 3MB

### 2.2 外设连接

| 外设 | 接口 | 引脚 | 说明 |
|------|------|------|------|
| 调试串口 | USART0 | PA9(TX)/PA10(RX) | 115200bps |
| ESP32 WiFi | USART2 | PA2(TX)/PA3(RX) | 115200bps |
| SHT30 温湿度 | I2C0 | PB6(SCL)/PB7(SDA) | 400KHz |
| MH-Z19B CO2 | UART3 | PB10(TX)/PB11(RX) | 9600bps |
| I2S 麦克风 | I2S0 | PA4(WS)/PA5(SCK)/PA6(SD) | 16KHz/16bit |
| PWM 扬声器 | TIM14 | PA7(CH0) | 44.1KHz |
| 继电器1 | GPIO | PB0 | 风扇控制 |
| 继电器2 | GPIO | PB1 | 加湿器控制 |
| 继电器3 | GPIO | PB2 | 灯光控制 |

### 2.3 接线示意图

```
GD32F470V-START
├── USART0 (PA9/PA10) ── USB-TTL ── PC (调试)
├── USART2 (PA2/PA3) ── ESP32 WiFi 模块
├── I2C0 (PB6/PB7) ── SHT30 温湿度传感器
├── UART3 (PB10/PB11) ── MH-Z19B CO2 传感器
├── I2S0 (PA4/PA5/PA6) ── I2S 麦克风模块
├── TIM14 (PA7) ── 有源蜂鸣器/扬声器
├── PB0 ── 继电器模块1 ── 风扇
├── PB1 ── 继电器模块2 ── 加湿器
└── PB2 ── 继电器模块3 ── 灯光
```

## 3. 软件配置

### 3.1 编译环境

```bash
# 设置工具链路径
export PATH="/home/tuya/openvela/prebuilts/gcc/linux-x86_64/arm-none-eabi/bin:$PATH"
export PATH="/home/tuya/openvela/prebuilts/build-tools/linux-x86_64/bin:$PATH"

# 进入项目目录
cd /home/tuya/openvela/nuttx
```

### 3.2 配置固件

```bash
# 清理旧配置
rm -f .config Make.defs

# 配置板级
tools/configure.sh -E /home/tuya/openvela/vendor/openvela/boards/contest2026_381_board/configs/nsh

# 修改配置（可选）
make menuconfig
```

### 3.3 编译固件

```bash
# 编译
make -j4

# 生成 bin 文件
arm-none-eabi-objcopy -O binary nuttx.elf nuttx.bin

# 查看固件大小
ls -lh nuttx.bin
```

### 3.4 烧录固件

使用 GD32 ISP 或 J-Link 烧录 `nuttx.bin` 到开发板。

## 4. 功能使用

### 4.1 启动系统

1. 烧录固件到开发板
2. 连接调试串口 (USART0)
3. 打开串口终端 (115200bps)
4. 上电启动

启动信息：
```
AI Butler: Starting...
AI Butler: Initializing modules...
Env Monitor: Initializing...
Timer Manager: Initializing...
Scene Manager: Initializing...
AI Butler: Ready!
AI Butler: Features enabled:
  - Continuous conversation
  - Fuzzy intent understanding
  - Proactive reminders
  - Scene automation
```

### 4.2 环境监测

系统自动监测环境数据：

```
Env Monitor: Temp=25.0°C, Humidity=50.0%, CO2=500ppm
```

### 4.3 告警功能

当环境异常时，系统自动告警：

```
⚠️ 温度过高：32.0°C (阈值：28.0°C)
AI Butler: Auto-opening fan due to high temperature
Env Monitor: Relay 1 (风扇) ON
```

### 4.4 定时提醒

系统自动问候：
- 08:00: "早上好！我是您的AI管家，祝您今天愉快！"
- 14:00: "下午好！记得适当休息哦。"
- 20:00: "晚上好！今天过得怎么样？"

### 4.5 场景联动

#### 起床场景 (07:00)
- 自动开灯
- 自动关风扇

#### 睡眠场景 (23:00)
- 自动关灯
- 自动开风扇
- 自动开加湿器

#### 工作场景 (手动)
- 开启灯光
- 关闭风扇和加湿器

#### 休息场景 (手动)
- 关闭灯光
- 开启风扇

## 5. 高级配置

### 5.1 配置 WiFi

修改 `cloud_connector.h` 中的 WiFi 配置：

```c
#define DEFAULT_WIFI_SSID       "your-wifi-ssid"
#define DEFAULT_WIFI_PASS       "your-wifi-password"
```

### 5.2 配置 LLM

修改 `cloud_connector.h` 中的 LLM 配置：

```c
#define DEEPSEEK_API_KEY       "your-api-key"
#define DEEPSEEK_MODEL         "deepseek-chat"
```

### 5.3 配置 ASR/TTS

修改 `cloud_connector.h` 中的火山引擎配置：

```c
#define VOLCENGINE_APP_ID       "your-app-id"
#define VOLCENGINE_ACCESS_TOKEN "your-access-token"
```

### 5.4 修改阈值

修改 `env_monitor.h` 中的环境阈值：

```c
#define TEMP_HIGH_THRESHOLD     28.0f   /* 温度高阈值 (°C) */
#define TEMP_LOW_THRESHOLD      16.0f   /* 温度低阈值 (°C) */
#define HUMIDITY_LOW_THRESHOLD  40.0f   /* 湿度低阈值 (%) */
#define HUMIDITY_HIGH_THRESHOLD 80.0f   /* 湿度高阈值 (%) */
#define CO2_HIGH_THRESHOLD      1000    /* CO2高阈值 (ppm) */
```

## 6. 故障排除

### 6.1 编译错误

**问题**: `FMC_PROGMEM_PAGESIZE undeclared`
**解决**: 检查 `CONFIG_GD32F4_FLASH_CONFIG_K=y` 是否启用

**问题**: `CONSOLE_UART undeclared`
**解决**: 检查 `CONFIG_GD32F4_USART0_SERIALDRIVER=y` 是否启用

### 6.2 运行错误

**问题**: 无输出
**解决**: 检查调试串口连接和波特率设置

**问题**: 传感器读取失败
**解决**: 检查传感器接线和地址配置

### 6.3 网络错误

**问题**: WiFi 连接失败
**解决**: 检查 SSID 和密码是否正确

**问题**: LLM 请求失败
**解决**: 检查 API Key 和网络连接

## 7. 技术支持

- **项目地址**: /home/tuya/openvela/
- **固件位置**: /home/tuya/openvela/nuttx/nuttx.bin
- **文档位置**: /home/tuya/openvela/vendor/openvela/boards/contest2026_381_board/
- **比赛信息**: 2026首届openvela AI硬件开发者大赛

## 8. 许可证

Apache License 2.0
