/****************************************************************************
 * AI Butler Board - Application Initialization
 * GD32F470V-START: board_appinit() and gd32_app_main()
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <debug.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Called from the OS early in initialization.
 *
 ****************************************************************************/

int board_app_initialize(uintptr_t arg)
{
#ifdef CONFIG_DEBUG_INFO
  board_info("AI Butler Board - Application Init\n");
#endif

  return OK;
}

/****************************************************************************
 * Name: gd32_app_main
 *
 * Description:
 *   This is the main entry point for the board application.
 *
 ****************************************************************************/

int gd32_app_main(int argc, char *argv[])
{
#ifdef CONFIG_DEBUG_INFO
  board_info("AI Butler Board - Starting NSH\n");
#endif

  /* Start NSH shell */
  return nsh_main(argc, argv);
}
