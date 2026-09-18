/****************************************************************************
 * AI 管家项目 - I2S 驱动 (MSM261S4030H0 数字麦克风)
 * GD32F470V-START: I2S0 (PA4=WS, PA5=SCK, PA6=SD)
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/audio/audio.h>
#include <debug.h>
#include <errno.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define I2S_WS_PIN      GPIO_PIN_4   /* PA4 - Word Select */
#define I2S_SCK_PIN     GPIO_PIN_5   /* PA5 - Serial Clock */
#define I2S_SD_PIN      GPIO_PIN_6   /* PA6 - Serial Data */

#define I2S_GPIO        GPIOA
#define I2S_GPIO_AF     GPIO_AF_5
#define I2S_GPIO_CLK    RCU_GPIOA

#define I2S_SPI         SPI0
#define I2S_SPI_CLK     RCU_SPI0

#define I2S_SAMPLE_RATE  16000
#define I2S_BITS         16
#define I2S_CHANNELS     1

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct audio_lowerhalf_s g_i2s_lower;
static bool g_i2s_initialized = false;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int i2s_txrx(struct audio_lowerhalf_s *lower, const void *tx,
                    void *rx, size_t nframes)
{
  /* I2S 传输/接收 */
  size_t bytes = nframes * (I2S_BITS / 8) * I2S_CHANNELS;

  if (tx)
    {
      /* 发送数据 */
      spi_i2s_data_transmit(I2S_SPI, (uint16_t *)tx);
    }

  if (rx)
    {
      /* 接收数据 */
      *(uint16_t *)rx = spi_i2s_data_receive(I2S_SPI);
    }

  return nframes;
}

static int i2s_ioctl(struct audio_lowerhalf_s *lower, int cmd,
                     unsigned long arg)
{
  switch (cmd)
    {
      case AUDIOIOC_START:
        {
          iinfo("I2S 启动\n");
          return OK;
        }

      case AUDIOIOC_STOP:
        {
          iinfo("I2S 停止\n");
          return OK;
        }

      case AUDIOIOC_SETFREQUENCY:
        {
          iinfo("I2S 采样率: %lu\n", (unsigned long)arg);
          return arg;
        }

      case AUDIOIOC_SETCHANNELS:
        {
          iinfo("I2S 通道数: %lu\n", (unsigned long)arg);
          return arg;
        }

      case AUDIOIOC_SETBITSPERSAMPLE:
        {
          iinfo("I2S 位宽: %lu\n", (unsigned long)arg);
          return arg;
        }

      default:
        return -ENOTTY;
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int board_i2sInitialize(void)
{
  if (g_i2s_initialized)
    {
      return OK;
    }

  /* 使能时钟 */
  rcu_periph_clock_enable(I2S_SPI_CLK);
  rcu_periph_clock_enable(I2S_GPIO_CLK);

  /* 配置 GPIO */
  gpio_af_set(I2S_GPIO, I2S_GPIO_AF,
              I2S_WS_PIN | I2S_SCK_PIN | I2S_SD_PIN);
  gpio_mode_set(I2S_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE,
                I2S_WS_PIN | I2S_SCK_PIN | I2S_SD_PIN);
  gpio_output_options_set(I2S_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          I2S_WS_PIN | I2S_SCK_PIN);

  /* SD 引脚为输入 */
  gpio_output_options_set(I2S_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                          I2S_SD_PIN);

  /* 配置 SPI 为 I2S 模式 */
  spi_i2s_struct_struct i2s_init;
  i2s_init.mode = SPI_I2S_MODE_MASTER;
  i2s_init.standard = SPI_I2SANDARD_PHILIPS;
  i2s_init.databits = SPI_I2S_DATABITS_16;
  i2s_init.clockpolarity = SPI_I2S_PLCKP_LOW;
  i2s_init.framesize = SPI_I2S_FRAMESIZE_16;

  /* 配置采样率 */
  i2s_init.audio_sampling = I2S_SAMPLE_RATE;

  /* 配置 I2S */
  i2s_init.i2s_prescaler = 10;  /* 根据系统时钟调整 */
  i2s_init.i2s_mckoutput = I2S_MCK_OUTPUT_DISABLE;

  /* 初始化 I2S */
  i2s_init_struct_init(&i2s_init);
  i2s_init(I2S_SPI, &i2s_init);

  /* 配置音频参数 */
  g_i2s_lower.ops = NULL;  /* 使用默认操作 */
  g_i2s_lower.private = NULL;

  g_i2s_initialized = true;
  iinfo("I2S 初始化成功: PA4=WS, PA5=SCK, PA6=SD (16kHz/16bit/mono)\n");
  return OK;
}

struct audio_lowerhalf_s *board_i2s_get_lower(void)
{
  return &g_i2s_lower;
}
