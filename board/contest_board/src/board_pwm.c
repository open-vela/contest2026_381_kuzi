/****************************************************************************
 * AI 管家项目 - PWM 驱动 (扬声器)
 * GD32F470V-START: PA7 (TIM14_CH0)
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/timers/pwm.h>
#include <debug.h>
#include <errno.h>

#include "gd32f4xx.h"
#include "gd32f4xx_timer.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PWM_TIMER      TIMER13
#define PWM_CHANNEL    TIMER_CH_0
#define PWM_GPIO       GPIOA
#define PWM_GPIO_PIN   GPIO_PIN_7
#define PWM_GPIO_AF    GPIO_AF_9
#define PWM_GPIO_CLK   RCU_GPIOA
#define PWM_TIMER_CLK  RCU_TIMER13

/****************************************************************************
 * Private Data
 ****************************************************************************/

static uint32_t g_pwm_duty = 0;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int board_pwmInitialize(void)
{
  /* 使能时钟 */
  rcu_periph_clock_enable(PWM_TIMER_CLK);
  rcu_periph_clock_enable(PWM_GPIO_CLK);

  /* 配置 GPIO 复用功能 */
  gpio_af_set(PWM_GPIO, PWM_GPIO_AF, PWM_GPIO_PIN);
  gpio_mode_set(PWM_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, PWM_GPIO_PIN);
  gpio_output_options_set(PWM_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          PWM_GPIO_PIN);

  /* 配置定时器 */
  timer_oc_parameter_struct timer_ocintpara;
  timer_parameter_struct timer_initpara;

  timer_deinit(PWM_TIMER);

  /* 时钟分频: APB2 = 100MHz, TIMER13 = 100MHz */
  timer_initpara.prescaler = 99;  /* 100MHz / 100 = 1MHz */
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 999;    /* 1MHz / 1000 = 1kHz */
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(PWM_TIMER, &timer_initpara);

  /* 配置 PWM 模式 */
  timer_ocintpara.ocstate = TIMER_OCSTATE_ENABLE;
  timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
  timer_ocintpara.ocnpolarity = TIMER_OC_POLARITY_HIGH;
  timer_channel_output_config(PWM_TIMER, PWM_CHANNEL, &timer_ocintpara);

  timer_channel_output_mode_config(PWM_TIMER, PWM_CHANNEL,
                                   TIMER_OC_MODE_PWM0);
  timer_channel_output_shadow_config(PWM_TIMER, PWM_CHANNEL,
                                     TIMER_OC_SHADOW_DISABLE);

  /* 初始占空比为 0 */
  timer_channel_output_pulse_value_config(PWM_TIMER, PWM_CHANNEL, 0);

  /* 使能定时器 */
  timer_enable(PWM_TIMER);

  iinfo("PWM 初始化成功: PA7 (TIM14_CH0, 1kHz)\n");
  return OK;
}

int board_pwm_set_duty(uint32_t duty_percent)
{
  if (duty_percent > 100)
    {
      return -EINVAL;
    }

  /* 计算比较值 */
  uint32_t period = 999;  /* 与 timer_initpara.period 一致 */
  uint32_t compare = (period * duty_percent) / 100;

  timer_channel_output_pulse_value_config(PWM_TIMER, PWM_CHANNEL, compare);
  g_pwm_duty = duty_percent;

  iinfo("PWM 占空比: %lu%%\n", (unsigned long)duty_percent);
  return OK;
}

int board_pwm_set_frequency(uint32_t freq_hz)
{
  if (freq_hz == 0)
    {
      return -EINVAL;
    }

  /* 重新配置定时器周期 */
  uint32_t prescaler = 99;  /* 1MHz 基频 */
  uint32_t period = (1000000 / freq_hz) - 1;

  timer_disable(PWM_TIMER);
  timer_autoreload_value_config(PWM_TIMER, period);
  timer_channel_output_pulse_value_config(PWM_TIMER, PWM_CHANNEL,
                                          (period * g_pwm_duty) / 100);
  timer_enable(PWM_TIMER);

  iinfo("PWM 频率: %lu Hz\n", (unsigned long)freq_hz);
  return OK;
}
