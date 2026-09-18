# AI 管家项目 - GD32F470V-START 板级支持
nuttx_add_library(board)
target_sources(board PRIVATE
  board_boot.c
  board_i2c.c
  board_uart2.c
  board_gpio.c
  board_pwm.c
  board_i2s.c
)
