# 板载守望 — 实现路径（无外设版）

> 核心板：GD32F470V-START  
> 范围：仅板载 LED / 按键 / 串口 CLI  
> 目标：可演示「主动巡检 + LED 执行」+ 3 个 Skill，满足提交最小闭环

## 功能

1. `ai_butler` NSH 应用：CLI 对话  
2. Skill：board-status / led-control / board-watch  
3. 主动：定时巡检空闲与堆；超时闪 LED2；按键闪 LED1  

## 非目标（已砍）

外接温湿度/CO₂、继电器、ESP32 WiFi、ASR/TTS、飞书/MQTT。

## 提交清单

- [ ] 代码合入专属仓 + logs/  
- [ ] 作品介绍文档  
- [ ] ≤5 分钟演示视频（板载场景即可）  
