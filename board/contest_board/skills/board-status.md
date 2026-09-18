# board-status Skill

## 名称
board-status

## 描述
查询 GD32F470V-START **板载**运行状态：uptime、堆内存、LED 掩码、按键计数。
不依赖任何外接传感器。

## 触发
- 用户输入 `status`
- 主动巡检时附带汇总

## 执行
打印结构化状态到控制台（串口 NSH）。
