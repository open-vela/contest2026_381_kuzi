# AI 管家 — 板载守望（无外设）

cmake_minimum_required(VERSION 3.0)

nuttx_add_application(
  NAME ai_butler
  PRIORITY 100
  STACKSIZE 4096
  SRCS
    src/ai_butler_main.c
    src/board_monitor.c
)
