# AI 管家硬件接线指南

## GD32F470V-START 引脚分配

| 功能 | MCU 引脚 | 接口 | 方向 | 连接说明 |
|------|----------|------|------|----------|
| Console TX | PA9 | USART0 | 输出 | USB-TTL 适配器 TX |
| Console RX | PA10 | USART0 | 输入 | USB-TTL 适配器 RX |
| ESP32 TX | PA2 | USART2 | 输出 | ESP32 RX |
| ESP32 RX | PA3 | USART2 | 输入 | ESP32 TX |
| SHT30 SCL | PB6 | I²C1 | 双向 | SHT30 SCL |
| SHT30 SDA | PB7 | I²C1 | 双向 | SHT30 SDA |
| I²S 麦克风 WS | PA4 | I²S0 | 输出 | MSM261S4030H0 WS |
| I²S 麦克风 SCK | PA5 | I²S0 | 输出 | MSM261S4030H0 SCK |
| I²S 麦克风 SD | PA6 | I²S0 | 输入 | MSM261S4030H0 SD |
| 继电器1 (灯) | PB0 | GPIO | 输出 | 继电器模块 IN1 |
| 继电器2 (风扇) | PB1 | GPIO | 输出 | 继电器模块 IN2 |
| 继电器3 (换气扇) | PB2 | GPIO | 输出 | 继电器模块 IN3 |
| PWM 扬声器 | PA7 | TIM14 | 输出 | 有源蜂鸣器/小喇叭 |

## 电源连接

| 电源 | 说明 |
|------|------|
| 3.3V | SHT30、ESP32（部分模块） |
| 5V | 继电器模块、I²S 麦克风（部分模块） |
| GND | 所有模块共地 |

## 接线图

```
GD32F470V-START
├── USART0 (PA9/PA10) ──→ USB-TTL 适配器 ──→ PC
├── USART2 (PA2/PA3) ──→ ESP32 WiFi 模块
├── I2C1 (PB6/PB7) ──→ SHT30 温湿度传感器
├── I2S0 (PA4/PA5/PA6) ──→ MSM261S4030H0 I²S 麦克风
├── GPIO (PB0/PB1/PB2) ──→ 3 路继电器模块
└── TIM14 (PA7) ──→ 有源蜂鸣器/小喇叭
```

## 注意事项

1. **电源**：确保所有模块共地，避免电平不一致
2. **I²C 上拉**：SHT30 需要 4.7KΩ 上拉电阻（部分模块自带）
3. **继电器驱动**：GPIO 驱动能力有限，建议使用光耦隔离
4. **I²S 麦克风**：确认模块支持 3.3V 电平（部分模块需要 5V）
5. **ESP32**：部分 ESP32 模块需要单独供电，注意电流需求
