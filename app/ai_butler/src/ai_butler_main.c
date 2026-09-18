/****************************************************************************
 * AI Butler — Board Sentinel (板载守望)
 *
 * 仅使用 GD32F470V-START 板载资源：
 *   - 用户 LED / 用户按键（有驱动则硬件，否则 soft 日志执行）
 *   - NSH/串口 CLI 对话
 *   - 定时主动巡检 + 阈值执行（满足「主动+执行」）
 *
 * 不依赖外接传感器、WiFi 模组、继电器、麦克风。
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#ifndef UNUSED
#  define UNUSED(x) ((void)(x))
#endif

#ifndef FAR
#  define FAR
#endif

#include "ai_butler.h"
#include "board_monitor.h"

#ifndef CONFIG_AI_BUTLER_INSPECTION_INTERVAL
#  define CONFIG_AI_BUTLER_INSPECTION_INTERVAL 15
#endif

#ifndef CONFIG_AI_BUTLER_IDLE_ALERT_SEC
#  define CONFIG_AI_BUTLER_IDLE_ALERT_SEC 20
#endif

#ifndef CONFIG_AI_BUTLER_STACKSIZE
#  define CONFIG_AI_BUTLER_STACKSIZE 4096
#endif

#ifndef CONFIG_AI_BUTLER_PRIORITY
#  define CONFIG_AI_BUTLER_PRIORITY 100
#endif

static bool g_running;

/* ---- tiny local "skills" (keyword router, board-only) ---- */

static void skill_board_status(void)
{
  struct board_status_s st;
  board_monitor_read(&st);
  printf("[skill:board-status]\n");
  printf("  uptime     : %lu s\n", (unsigned long)st.uptime_sec);
  printf("  free heap  : %lu B\n", (unsigned long)st.free_heap);
  printf("  largest    : %lu B\n", (unsigned long)st.largest_heap);
  printf("  led mask   : 0x%02x\n", st.led_mask);
  printf("  button hits: %lu\n", (unsigned long)st.button_hits);
  printf("  idle       : %lu s\n", (unsigned long)st.idle_sec);
}

static void skill_led_control(const char *args)
{
  /* led <1|2|3> <on|off|blink> */
  int id = 1;
  char act[16] = {0};

  if (args == NULL || *args == '\0')
    {
      printf("usage: led <1|2|3> <on|off|blink>\n");
      return;
    }

  sscanf(args, "%d %15s", &id, act);
  if (id < 1 || id > 3)
    {
      printf("led id must be 1..3\n");
      return;
    }

  if (strcmp(act, "on") == 0)
    {
      board_monitor_set_led(id - 1, true);
    }
  else if (strcmp(act, "off") == 0)
    {
      board_monitor_set_led(id - 1, false);
    }
  else if (strcmp(act, "blink") == 0)
    {
      board_monitor_blink(id - 1, 5);
    }
  else
    {
      printf("usage: led <1|2|3> <on|off|blink>\n");
    }
}

static void skill_dialog_help(void)
{
  printf("[skill:dialog-manager] board-only commands:\n");
  printf("  help / ?          show this help\n");
  printf("  status            board-status skill\n");
  printf("  led N on|off|blink\n");
  printf("  inspect           run proactive inspection now\n");
  printf("  quit / exit       stop agent\n");
}

static void proactive_inspect(bool force)
{
  struct board_status_s st;
  board_monitor_read(&st);

  printf("[skill:board-watch][proactive] inspect uptime=%lus heap=%lu idle=%lus\n",
         (unsigned long)st.uptime_sec,
         (unsigned long)st.free_heap,
         (unsigned long)st.idle_sec);

  /* Threshold: idle too long -> execute LED alert */
  if (force || st.idle_sec >= (uint32_t)CONFIG_AI_BUTLER_IDLE_ALERT_SEC)
    {
      printf("[skill:board-watch][execute] idle>=%ds -> blink LED2 as alert\n",
             CONFIG_AI_BUTLER_IDLE_ALERT_SEC);
      board_monitor_blink(1, 4);
      board_monitor_note_activity(); /* avoid alert storm */
    }

  /* Soft heap pressure demo threshold (very low bar for demo) */
  if (st.free_heap > 0 && st.free_heap < 2048)
    {
      printf("[skill:board-watch][execute] low heap -> LED3 on\n");
      board_monitor_set_led(2, true);
    }
}

static void handle_line(char *line)
{
  char *p = line;
  while (*p && isspace((unsigned char)*p))
    {
      p++;
    }

  if (*p == '\0')
    {
      return;
    }

  board_monitor_note_activity();

  if (!strcmp(p, "help") || !strcmp(p, "?"))
    {
      skill_dialog_help();
    }
  else if (!strcmp(p, "status"))
    {
      skill_board_status();
    }
  else if (!strncmp(p, "led", 3) && (p[3] == '\0' || isspace((unsigned char)p[3])))
    {
      skill_led_control(p + 3);
    }
  else if (!strcmp(p, "inspect"))
    {
      proactive_inspect(true);
    }
  else if (!strcmp(p, "quit") || !strcmp(p, "exit"))
    {
      g_running = false;
    }
  else
    {
      printf("unknown: %s  (try help)\n", p);
    }
}

static int ai_butler_task(int argc, FAR char *argv[])
{
  char line[96];
  int inspec_left = CONFIG_AI_BUTLER_INSPECTION_INTERVAL;

  UNUSED(argc);
  UNUSED(argv);

  printf("\n");
  printf("========================================\n");
  printf("  AI Butler / Board Sentinel\n");
  printf("  GD32F470V-START 板载守望 (无外设)\n");
  printf("========================================\n");
  printf("Skills: board-status / led-control / board-watch\n");
  printf("Proactive interval: %ds, idle alert: %ds\n",
         CONFIG_AI_BUTLER_INSPECTION_INTERVAL,
         CONFIG_AI_BUTLER_IDLE_ALERT_SEC);
  printf("Type 'help' for commands.\n\n");

  board_monitor_init();
  skill_dialog_help();
  g_running = true;

  while (g_running)
    {
      struct pollfd pfd;
      int pret;

      board_monitor_tick();

      pfd.fd = STDIN_FILENO;
      pfd.events = POLLIN;
      pret = poll(&pfd, 1, 1000);

      if (pret > 0 && (pfd.revents & POLLIN))
        {
          if (fgets(line, sizeof(line), stdin) != NULL)
            {
              size_t n = strlen(line);
              while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r'))
                {
                  line[--n] = '\0';
                }

              handle_line(line);
            }
        }

      if (--inspec_left <= 0)
        {
          inspec_left = CONFIG_AI_BUTLER_INSPECTION_INTERVAL;
          proactive_inspect(false);
        }
    }

  printf("[board] agent stopped\n");
  return OK;
}

int ai_butler_start(void)
{
  int ret = task_create("ai_butler", CONFIG_AI_BUTLER_PRIORITY,
                        CONFIG_AI_BUTLER_STACKSIZE, ai_butler_task, NULL);
  return ret < 0 ? -errno : OK;
}

int ai_butler_stop(void)
{
  g_running = false;
  return OK;
}

/****************************************************************************
 * NSH builtin entry: ai_butler
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  return ai_butler_task(1, NULL);
}
