#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/lp_core_ll.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_i2c.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "../../../misc.c"

#define ADXL367_I2C_ADDR 0x1D

int32_t value_x[12];
int32_t value_y[12];
int32_t value_z[12];

int conv(char * ary, int base) {
  return (((int)ary[base] << 26) >> 18) + ary[base+1];
}

const char FIFO_READ[] = {0x18};
int sensor_read(int idx) {
  char data_rd[6];
  lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, ADXL367_I2C_ADDR, FIFO_READ, sizeof(FIFO_READ), -1);
  delayMs(5);
  if(lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, ADXL367_I2C_ADDR, data_rd, sizeof(data_rd), -1) != ESP_OK) return 1;
  value_x[idx] = conv(data_rd, 0);
  value_y[idx] = conv(data_rd, 2);
  value_z[idx] = conv(data_rd, 4);
  return 0;
}

const char CMD_MEASURE[] = {0x2D, 2};
const char CMD_STANDBY[] = {0x2D, 0};

int sensor_on(void) {
  lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, ADXL367_I2C_ADDR, CMD_MEASURE, sizeof(CMD_MEASURE), -1);
}
int sensor_off(void) {
  lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, ADXL367_I2C_ADDR, CMD_STANDBY, sizeof(CMD_STANDBY), -1);
}

int read_for_1sec(void) {
  int res = 0;
  sensor_on();
  delayMs(20);
  for(int i = 0; i < 12; ++i) {
    delayMs(80);
    if(sensor_read(i)) res = 1;
  }
  sensor_off();
  return res;
}

#define GRAVITY 4000
#define THRESHOLD (INT_MAX-1)

// !!! CAUTION
// The for loop in the main function breaks value_x,y,and z.
// So I set threshold as a very high.
// This is a gcc's bug or an ESP's bug?
int app_main (void)
{
    while(1) {
      if(read_for_1sec()) goto FAIL;
      int vx = 0, vy = 0, vz = 0;
      for(int i = 0; i < 12; ++i) {
        vx += value_x[i];
        vy += value_y[i];
        vz += value_z[i];
      }
      vx = (abs(vx) + abs(vy) + abs(vz))/12;
      if(abs(vx - GRAVITY) > THRESHOLD) goto FAIL;
      delayMs(7*1000);
    }
    return 0;
 FAIL:
    wakeup();
    return 0;
}
