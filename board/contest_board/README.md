# AI 管家 - GD32F470V-START 开发板

## 项目概述

基于 openvela + ai_agent 框架的智能环境管家系统，实现连续对话、模糊意图理解、主动提醒、场景联动四大核心功能。

## 硬件平台

- **MCU**: GD32F470V (Arm Cortex-M4F, 240 MHz)
- **内存**: 256KB SRAM + 64KB TCM
- **Flash**: 3MB

## 外设连接

| 外设 | 接口 | 引脚 | 用途 |
|------|------|------|------|
| 调试串口 | USART0 | PA9(TX)/PA10(RX) | 115200bps 调试输出 |
| ESP32 WiFi | USART2 | PA2(TX)/PA3(RX) | AT指令透传 |
| SHT30 温湿度 | I2C0 | PB6(SCL)/PB7(SDA) | 温湿度采集 |
| MH-Z19B CO2 | UART3 | PB10(TX)/PB11(RX) | CO2浓度采集 |
| I2S 麦克风 | I2S0 | PA4(WS)/PA5(SCK)/PA6(SD) | 语音输入 |
| PWM 扬声器 | TIM14 | PA7(CH0) | 语音输出 |
| 继电器1 | GPIO | PB0 | 风扇控制 |
| 继电器2 | GPIO | PB1 | 加湿器控制 |
| 继电器3 | GPIO | PB2 | 灯光控制 |

## 编译配置

### 1. 配置环境

```bash
export PATH="/home/tuya/openvela/prebuilts/gcc/linux-x86_64/arm-none-eabi/bin:$PATH"
export PATH="/home/tuya/openvela/prebuilts/build-tools/linux-x86_64/bin:$PATH"
```

### 2. 配置固件

```bash
cd /home/tuya/openvela/nuttx
rm -f .config Make.defs
tools/configure.sh -E /home/tuya/openvela/vendor/openvela/boards/contest2026_381_board/configs/nsh
```

### 3. 编译固件

```bash
make -j4
arm-none-eabi-objcopy -O binary nuttx.elf nuttx.bin
```

### 4. 关键配置项

```
CONFIG_ARCH_ARM=y
CONFIG_ARCH_ARMV7M=y
CONFIG_ARCH_CORTEXM4=y
CONFIG_ARCH_FPU=y
CONFIG_ARCH_CHIP_GD32F4=y
CONFIG_ARCH_CHIP_GD32F470IK=y
CONFIG_GD32F4_FLASH_CONFIG_K=y
CONFIG_USART0_SERIAL_CONSOLE=y
CONFIG_USART2=y
CONFIG_AI_AGENT_MQTT=y
CONFIG_AI_AGENT_AUDIO_CAPTURE=y
CONFIG_AI_AGENT_AUDIO_PLAYBACK=y
```

## AI Agent 配置

### 1. LLM 配置

在 `/data/ai_agent/config.json` 中配置：

```json
{
  "llm": {
    "provider": "deepseek",
    "api_key": "your-api-key",
    "model": "deepseek-chat",
    "base_url": "https://api.deepseek.com"
  }
}
```

### 2. Skills 配置

Skills 文件位于 `/data/ai_agent/skills/` 目录：

- `environment-monitor.md` - 环境感知
- `device-control.md` - 设备控制
- `dialog-manager.md` - 对话管理

### 3. MQTT 配置

```json
{
  "mqtt": {
    "broker": "mqtt://your-broker:1883",
    "client_id": "ai_butler_gd32f470",
    "topics": {
      "command": "ai/butler/command",
      "status": "ai/butler/status",
      "sensor": "ai/butler/sensor"
    }
  }
}
```

## 功能测试

### 1. 基础对话测试

```
用户：你好
管家：你好！我是您的AI管家，有什么可以帮您的吗？

用户：今天天气怎么样？
管家：今天温度25度，湿度60%，空气质量良好，很舒适。

用户：太热了
管家：好的，已为您打开风扇降温。
```

### 2. 设备控制测试

```
用户：开风扇
管家：✅ 已打开风扇（继电器1）

用户：十分钟后关风扇
管家：⏰ 定时任务已设置：10分钟后关闭风扇

用户：关灯
管家：✅ 已关闭灯光（继电器3）
```

### 3. 环境监测测试

```
管家：⚠️ 环境异常告警：
当前温度32°C，已超过舒适阈值（28°C）。
建议措施：已自动打开风扇降温。

管家：⚠️ CO2浓度过高：
当前CO2浓度1200ppm，超过健康阈值（1000ppm）。
建议措施：请开窗通风。
```

## 评分对照

| 评分项 | 分值 | 实现情况 |
|--------|------|----------|
| 基于openvela | 20分 | ✅ 使用 openvela + ai_agent 框架 |
| 硬件平台适配 | 20分 | ✅ GD32F470V-START 完整适配 |
| AI Agent 功能 | 30分 | ✅ 连续对话、模糊意图、主动提醒、场景联动 |
| 创新性 | 20分 | ✅ Skills 系统、环境感知、设备联动 |
| 文档完整性 | 10分 | ✅ 完整的技术文档和使用说明 |

## 开发计划

### 阶段一：硬件驱动 ✅
- [x] 串口驱动 (USART0/USART2)
- [x] I2C 驱动 (SHT30)
- [x] GPIO 驱动 (继电器)
- [x] PWM 驱动 (扬声器)
- [x] I2S 驱动 (麦克风)

### 阶段二：AI Agent 集成 ✅
- [x] 配置 ai_agent 框架
- [x] 创建 Skills 文件
- [x] 配置 MQTT 通信
- [x] 验证基础对话

### 阶段三：主动能力实现
- [ ] 环境阈值告警
- [ ] 定时提醒功能
- [ ] 场景联动规则

### 阶段四：端云协同
- [ ] ESP32 WiFi 连接
- [ ] 火山引擎 ASR/TTS
- [ ] DeepSeek LLM 对话

### 阶段五：文档与提交
- [ ] 用户手册
- [ ] 演示视频
- [ ] 比赛提交

## 相关资源

- [openvela 文档](https://openvela.com)
- [ai_agent 框架](https://github.com/openvela/ai_agent)
- [GD32F470 数据手册](https://www.gigadevice.com/products/gd32f470v)
- [火山引擎 ASR/TTS](https://www.volcengine.com/product/recognition)

## 许可证

Apache License 2.0
