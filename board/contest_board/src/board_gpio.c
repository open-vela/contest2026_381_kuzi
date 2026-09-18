/****************************************************************************
 * AI 管家项目 - GPIO 驱动 (继电器控制)
 * GD32F470V-START: PB0=继电器1(灯), PB1=继电器2(风扇), PB2=继电器3(换气扇)
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/ioexpander/gpio.h>
#include <debug.h>
#include <errno.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RELAY1_PIN    GPIO_PIN_0   /* PB0 - 客厅灯 */
#define RELAY2_PIN    GPIO_PIN_1   /* PB1 - 风扇 */
#define RELAY3_PIN    GPIO_PIN_2   /* PB2 - 换气扇 */

#define RELAY_PORT    GPIOB
#define RELAY_PORT_CLK RCU_GPIOB

/****************************************************************************
 * Private Data
 ****************************************************************************/

static bool g_relay_state[3] = {false, false, false};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int board_gpioInitialize(void)
{
  /* 使能 GPIOB 时钟 */
  rcu_periph_clock_enable(RELAY_PORT_CLK);

  /* 配置 GPIO 为推挽输出 */
  gpio_mode_set(RELAY_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                RELAY1_PIN | RELAY2_PIN | RELAY3_PIN);
  gpio_output_options_set(RELAY_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          RELAY1_PIN | RELAY2_PIN | RELAY3_PIN);

  /* 初始化为低电平（继电器断开） */
  gpio_bit_reset(RELAY_PORT, RELAY1_PIN | RELAY2_PIN | RELAY3_PIN);

  iinfo("GPIO 初始化成功: PB0=继电器1, PB1=继电器2, PB2=继电器3\n");
  return OK;
}

int board_relay_control(int relay, bool state)
{
  uint16_t pin;

  switch (relay)
    {
      case 0:
        pin = RELAY1_PIN;
        break;
      case 1:
        pin = RELAY2_PIN;
        break;
      case 2:
        pin = RELAY3_PIN;
        break;
      default:
        return -EINVAL;
    }

  if (state)
    {
      gpio_bit_set(RELAY_PORT, pin);
    }
  else
    {
      gpio_bit_reset(RELAY_PORT, pin);
    }

  g_relay_state[relay] = state;
  iinfo("继电器 %d: %s\n", relay + 1, state ? "ON" : "OFF");
  return OK;
}

bool board_relay_get_state(int relay)
{
  if (relay < 0 || relay > 2)
    return false;

  return g_relay_state[relay];
}
