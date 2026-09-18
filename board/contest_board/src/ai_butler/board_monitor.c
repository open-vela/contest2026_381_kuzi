/****************************************************************************
 * Board-only monitor implementation (GD32F470V-START on-board I/O)
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <time.h>

#include <nuttx/leds/userled.h>
#include <nuttx/input/buttons.h>

#include "board_monitor.h"

#ifndef CONFIG_AI_BUTLER_IDLE_ALERT_SEC
#  define CONFIG_AI_BUTLER_IDLE_ALERT_SEC 20
#endif

static struct board_status_s g_st;
static int g_led_fd = -1;
static int g_btn_fd = -1;
static uint8_t g_btn_prev;
static time_t g_boot;
static time_t g_last_cmd;

int board_monitor_init(void)
{
  memset(&g_st, 0, sizeof(g_st));
  g_boot = time(NULL);
  g_last_cmd = g_boot;

#ifdef CONFIG_USERLED
  g_led_fd = open("/dev/userleds", O_WRONLY);
#endif

#ifdef CONFIG_INPUT_BUTTONS
  g_btn_fd = open("/dev/buttons", O_RDONLY | O_NONBLOCK);
#endif

  printf("[board] monitor ready (leds=%s buttons=%s)\n",
         g_led_fd >= 0 ? "hw" : "soft",
         g_btn_fd >= 0 ? "hw" : "soft");
  return OK;
}

void board_monitor_note_activity(void)
{
  g_last_cmd = time(NULL);
  g_st.idle_sec = 0;
}

void board_monitor_tick(void)
{
  time_t now = time(NULL);
  g_st.uptime_sec = (uint32_t)(now - g_boot);
  g_st.idle_sec = (uint32_t)(now - g_last_cmd);

  struct mallinfo mi = mallinfo();
  g_st.free_heap = (uint32_t)mi.fordblks;
  g_st.largest_heap = (uint32_t)mi.mxordblk;

  board_monitor_poll_button();
}

void board_monitor_poll_button(void)
{
  uint8_t sample = 0;

  if (g_btn_fd >= 0)
    {
      btn_buttonset_t set = 0;
      ssize_t n = read(g_btn_fd, &set, sizeof(set));
      if (n == (ssize_t)sizeof(set))
        {
          sample = (uint8_t)(set & 0xff);
        }
    }

  /* Rising edge on any button bit */
  uint8_t rose = (uint8_t)(sample & (uint8_t)(~g_btn_prev));
  if (rose)
    {
      g_st.button_hits++;
      board_monitor_note_activity();
      printf("[board][proactive] button event mask=0x%02x hits=%lu -> blink LED1\n",
             rose, (unsigned long)g_st.button_hits);
      board_monitor_blink(0, 3);
    }

  g_btn_prev = sample;
}

int board_monitor_read(struct board_status_s *out)
{
  if (!out)
    {
      return -EINVAL;
    }

  board_monitor_tick();
  *out = g_st;
  return OK;
}

static int apply_leds(void)
{
  if (g_led_fd < 0)
    {
      return OK; /* soft mode */
    }

#ifdef CONFIG_USERLED
  userled_set_t set = (userled_set_t)g_st.led_mask;
  return ioctl(g_led_fd, ULEDIOC_SETALL, (unsigned long)set);
#else
  return OK;
#endif
}

int board_monitor_set_led(int led, bool on)
{
  if (led < 0 || led > 2)
    {
      return -EINVAL;
    }

  if (on)
    {
      g_st.led_mask |= (uint8_t)(1u << led);
    }
  else
    {
      g_st.led_mask &= (uint8_t)~(1u << led);
    }

  printf("[board][exec] LED%d %s (mask=0x%02x)\n",
         led + 1, on ? "ON" : "OFF", g_st.led_mask);
  return apply_leds();
}

int board_monitor_blink(int led, int times)
{
  int i;
  if (times < 1)
    {
      times = 1;
    }

  for (i = 0; i < times; i++)
    {
      board_monitor_set_led(led, true);
      usleep(120000);
      board_monitor_set_led(led, false);
      usleep(120000);
    }

  return OK;
}
