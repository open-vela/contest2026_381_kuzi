/****************************************************************************
 * AI 管家项目 - UART2 驱动 (ESP32 WiFi 模块)
 * GD32F470V-START: USART2 (PA2=TX, PA3=RX)
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/serial/serial.h>
#include <debug.h>
#include <errno.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define ESP32_UART_BAUD     115200
#define ESP32_UART.databits  8
#define ESP32_UART_parity    0
#define ESP32_UART_stopbits  1

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct uart_dev_s g_esp32_uart;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int esp32_setup(struct uart_dev_s *dev)
{
  /* 配置 USART2 时钟 */
  rcu_periph_clock_enable(RCU_USART2);

  /* 配置 GPIO */
  gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2);  /* PA2 -> USART2_TX */
  gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_3);  /* PA3 -> USART2_RX */

  /* 配置 USART2 */
  usart_deinit(USART2, ESP32_UART_BAUD);
  usart_word_length_set(USART2, USART_WL_8BIT);
  usart_stop_bit_length_set(USART2, USART_STB_1BIT);
  usart_parity_config(USART2, USART_PM_NONE);
  usart_hardware_flow_control_config(USART2, USART_HardwareFlowNone);

  /* 使能发送和接收 */
  usart_enable(USART2);
  usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
  usart_receive_config(USART2, USART_RECEIVE_ENABLE);

  return OK;
}

static void esp32_shutdown(struct uart_dev_s *dev)
{
  usart_disable(USART2);
}

static int esp32_send(struct uart_dev_s *dev, int ch)
{
  while (usart_flag_get(USART2, USART_FLAG_TBE) == RESET)
    {
    }

  usart_data_transmit(USART2, (uint8_t)ch);
  return ch;
}

static int esp32_receive(struct uart_dev_s *dev)
{
  while (usart_flag_get(USART2, USART_FLAG_RBNE) == RESET)
    {
    }

  return (int)usart_data_receive(USART2);
}

static bool esp32_rxavailable(struct uart_dev_s *dev)
{
  return usart_flag_get(USART2, USART_FLAG_RBNE) != RESET;
}

/****************************************************************************
 * Public Data
 ****************************************************************************/

const struct uart_ops_s g_esp32_uarts_ops =
{
  .setup    = esp32_setup,
  .shutdown = esp32_shutdown,
  .send     = esp32_send,
  .receive  = esp32_receive,
  .rxavailable = esp32_rxavailable,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int board_uart2Initialize(void)
{
  /* 配置 USART2 */
  g_esp32_uart.isconsole = false;
  g_esp32_uart.ops = &g_esp32_uarts_ops;

  /* 注册串口设备 */
  int ret = uart_register("/dev/ttyS2", &g_esp32_uart);
  if (ret < 0)
    {
      ierr("USART2 注册失败: %d\n", ret);
      return ret;
    }

  iinfo("USART2 初始化成功 (PA2=TX, PA3=RX, 115200 8N1)\n");
  return OK;
}

/****************************************************************************
 * ESP32 AT 指令接口
 ****************************************************************************/

int board_esp32_send_cmd(const char *cmd, char *resp, size_t resp_len)
{
  int fd;
  int n;

  fd = open("/dev/ttyS2", O_RDWR);
  if (fd < 0)
    {
      ierr("打开 ESP32 串口失败: %d\n", errno);
      return -errno;
    }

  /* 发送 AT 指令 */
  write(fd, cmd, strlen(cmd));
  write(fd, "\r\n", 2);

  /* 接收响应（超时 5s） */
  struct pollfd fds[1];
  fds[0].fd = fd;
  fds[0].events = POLLIN;

  int ret = poll(fds, 1, 5000);
  if (ret > 0 && (fds[0].revents & POLLIN))
    {
      n = read(fd, resp, resp_len - 1);
      if (n > 0)
        {
          resp[n] = '\0';
        }
    }
  else
    {
      n = -ETIMEDOUT;
    }

  close(fd);
  return n;
}

int board_esp32_connect_wifi(const char *ssid, const char *pwd)
{
  char resp[128];
  char cmd[128];

  /* 复位 ESP32 */
  board_esp32_send_cmd("AT+RST", resp, sizeof(resp));
  usleep(2000000);  /* 等待 2s */

  /* 设置 STA 模式 */
  board_esp32_send_cmd("AT+CWMODE=1", resp, sizeof(resp));

  /* 连接 WiFi */
  snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
  return board_esp32_send_cmd(cmd, resp, sizeof(resp));
}
