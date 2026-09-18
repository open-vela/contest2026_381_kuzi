/****************************************************************************
 * AI 管家项目 - I2C 驱动 (SHT30 温湿度传感器)
 * GD32F470V-START: I2C1 (PB6=SCL, PB7=SDA)
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/i2c/i2c_master.h>
#include <debug.h>
#include <errno.h>

#include "gd32f4xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SHT30_I2C_ADDR       0x44  /* SHT30 I2C 地址 */
#define SHT30_CMD_MEASURE_H  0x2400  /* 高重复性测量命令 */

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct i2c_master_s *g_sht30_i2c;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * 初始化 I2C 总线
 ****************************************************************************/

int board_i2cInitialize(void)
{
  /* 初始化 I2C1 */
  g_sht30_i2c = gd32_i2c_initialize(1);
  if (!g_sht30_i2c)
    {
      ierr("I2C1 初始化失败\n");
      return -ENODEV;
    }

  iinfo("I2C1 初始化成功 (PB6=SCL, PB7=SDA)\n");
  return OK;
}

/****************************************************************************
 * 读取 SHT30 温湿度
 ****************************************************************************/

int board_sht30_read(float *temperature, float *humidity)
{
  struct i2c_msg_s msg[2];
  uint8_t cmd[2];
  uint8_t data[6];
  int ret;

  if (!g_sht30_i2c || !temperature || !humidity)
    return -EINVAL;

  /* 发送测量命令 */
  cmd[0] = (SHT30_CMD_MEASURE_H >> 8) & 0xFF;
  cmd[1] = SHT30_CMD_MEASURE_H & 0xFF;

  msg[0].frequency = I2C_FREQUENCY_400KHZ;
  msg[0].addr = SHT30_I2C_ADDR;
  msg[0].flags = I2C_M_NOSTOP;
  msg[0].buffer = cmd;
  msg[0].length = 2;

  ret = I2C_TRANSFER(g_sht30_i2c, msg, 1);
  if (ret < 0)
    {
      ierr("SHT30 发送命令失败: %d\n", ret);
      return ret;
    }

  /* 等待测量完成 */
  usleep(20000);  /* 20ms */

  /* 读取数据 */
  msg[0].frequency = I2C_FREQUENCY_400KHZ;
  msg[0].addr = SHT30_I2C_ADDR;
  msg[0].flags = I2C_M_READ;
  msg[0].buffer = data;
  msg[0].length = 6;

  ret = I2C_TRANSFER(g_sht30_i2c, msg, 1);
  if (ret < 0)
    {
      ierr("SHT30 读取数据失败: %d\n", ret);
      return ret;
    }

  /* 解析温湿度数据 */
  uint16_t raw_temp = (data[0] << 8) | data[1];
  uint16_t raw_humi = (data[3] << 8) | data[4];

  *temperature = -45.0f + 175.0f * (float)raw_temp / 65535.0f;
  *humidity = 100.0f * (float)raw_humi / 65535.0f;

  iinfo("SHT30: 温度=%.1f°C, 湿度=%.1f%%\n", *temperature, *humidity);
  return OK;
}
