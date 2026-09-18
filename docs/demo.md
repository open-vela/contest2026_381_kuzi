# 演示脚本（板载守望 · 无外设）

1. 烧录固件，串口 115200 进入 NSH  
2. `ai_butler`  
3. `help` → 展示 Skill  
4. 等待约 20s 不操作 → 主动空闲告警闪灯  
5. `status` / `led 1 blink`  
6. 按用户键 → 主动闪 LED1  
7. `quit`

全程不需要传感器、WiFi、继电器。
