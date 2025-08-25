#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/lp_core_ll.h"
#include "riscv/rv_utils.h"
#include "riscv/rvruntime-frames.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_i2c.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "ulp_lp_core_interrupts.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "../../../misc.c"

#define NUM 30

#define SHT30_I2C_ADDR 0x44
const char LOW_REPEAT_READ[] = {0x24, 0x16};

uint32_t tmp[NUM];
uint32_t rh[NUM];


void read_sensor(int index) {
    char data_rd[6];
    lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, LOW_REPEAT_READ, sizeof(LOW_REPEAT_READ), -1);
    delayMs(6);
    lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, data_rd, sizeof(data_rd), -1);
    int t_ticks = (data_rd[0] << 8) + data_rd[1];
    int rh_ticks = (data_rd[3] << 8) + data_rd[4];
    tmp[index] = ((t_ticks * 175)/0xFFFF) - 45;
    int rhv = ((rh_ticks * 100)/0xFFFF);
    if(rhv > 100) {
        rhv = 100;
    } else if(rhv < 0) {
        rhv = 0;
    }
    rh[index] = rhv;
}

int counter = 0;
int app_main(void) {
    for(int i = 0;i < NUM; ++i) {
      read_sensor(i);
      delayMs(60*1000-6);
    }
    return 0;
}
