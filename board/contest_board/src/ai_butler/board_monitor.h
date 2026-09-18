/****************************************************************************
 * Board-only monitor — no external sensors required
 ****************************************************************************/

#ifndef __APPS_AI_BUTLER_BOARD_MONITOR_H
#define __APPS_AI_BUTLER_BOARD_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

struct board_status_s
{
  uint32_t uptime_sec;
  uint32_t free_heap;
  uint32_t largest_heap;
  uint8_t  led_mask;       /* bit0=LED1 ... */
  uint32_t button_hits;    /* cumulative user-key edges */
  uint32_t idle_sec;       /* seconds since last CLI command */
};

int  board_monitor_init(void);
void board_monitor_tick(void);          /* call ~1 Hz */
void board_monitor_note_activity(void); /* CLI used */
int  board_monitor_read(struct board_status_s *out);
int  board_monitor_set_led(int led, bool on); /* led 0..2 */
int  board_monitor_blink(int led, int times);
void board_monitor_poll_button(void);

#endif
