/****************************************************************************
 * AI 管家项目 - GD32F470V-START 板级初始化
 * 基于 openvela + ai_agent 框架
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/arch.h>
#include <debug.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void gd32_boardinitialize(void)
{
#ifdef CONFIG_DEBUG_INFO
  board_info("AI 管家 - GD32F470V-START 板级初始化\n");
  board_info("MCU: Arm Cortex-M4F @ 240 MHz\n");
  board_info("SRAM: 256 KB + 64 KB TCM\n");
#endif

  /* 配置 GPIO 等硬件 */
  /* TODO: 添加硬件初始化代码 */

#ifdef CONFIG_DEBUG_INFO
  board_info("板级初始化完成\n");
#endif
}

/****************************************************************************
 * Name: main
 *
 * Description:
 *   This is the main entry point for the firmware.
 *
 ****************************************************************************/

int main(int argc, char *argv[])
{
#ifdef CONFIG_DEBUG_INFO
  board_info("AI Butler Board - Starting...\n");
#endif

  /* 启动 NuttX */
  nx_start();

  /* 不应该到达这里 */
  return 0;
}
