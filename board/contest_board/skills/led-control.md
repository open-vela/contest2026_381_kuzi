# led-control Skill

## 名称
led-control

## 描述
控制板载用户 LED1/LED2/LED3（有 `/dev/userleds` 则驱动硬件，否则 soft 日志执行）。

## 触发
- `led <1|2|3> on|off|blink`

## 执行
点亮 / 熄灭 / 闪烁对应 LED，作为 Agent「会执行」的板载执行器。
