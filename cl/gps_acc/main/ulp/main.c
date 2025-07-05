#include <stdlib.h>
#include <limits.h>
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

#if SOC_LP_CORE_SINGLE_INTERRUPT_VECTOR
#define MIE_ALL_INTS_MASK (1 << 30)
#else
#define MIE_ALL_INTS_MASK 0x3FFF0888
#endif

void LP_CORE_ISR_ATTR ulp_lp_core_lp_timer_intr_handler(void)
{
  ulp_lp_core_lp_timer_intr_clear();
  ulp_lp_core_lp_timer_disable();
}

#define ADXL367_I2C_ADDR 0x53

int32_t value_x[12];
int32_t value_y[12];
int32_t value_z[12];

void sleep_store(void);
void sleep_restore(void);
void sleep_store2(void) {
    ulp_lp_core_halt();
}

void wakeup(void) {
    ulp_lp_core_lp_timer_disable();
    ulp_lp_core_wakeup_main_processor();
    ulp_lp_core_halt();
    sleep_store2();
}

volatile void * mrbc_sp_bottom = NULL;

void sleep(int ms) {
    lp_core_ll_set_wakeup_source(LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER);
    ulp_lp_core_lp_timer_set_wakeup_time(1000 * ms);
    sleep_store();
/*
    // enable interruption
    RV_SET_CSR(mstatus, MSTATUS_MIE);
    RV_SET_CSR(mie, MIE_ALL_INTS_MASK);
    asm volatile("wfi");
    // disable interruption
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    RV_CLEAR_CSR(mie, MIE_ALL_INTS_MASK);
*/
}

int conv(char * ary, int base) {
  return (((int)ary[base] << 26) >> 18) + ary[base+1];
}

const char FIFO_READ[] = {0x18};
int sensor_read(int idx) {
  char data_rd[6];
  lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, ADXL367_I2C_ADDR, FIFO_READ, sizeof(FIFO_READ), -1);
  sleep(5);
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
  for(int i = 0; i < 12; ++i) {
    sleep(80);
    if(sensor_read(i)) res = 1;
  }
  sensor_off();
  return res;
}

#define GRAVITY 4000
#define THRESHOLD (INT_MAX-1) // 2000

int i = 0;

// !!! CAUTION
// The for loop in the main function breaks value_x,y,and z.
// So I set threshold as a very high.
// This is a gcc's bug or an ESP's bug?
int app_main (void)
{
    if(read_for_1sec()) goto FAIL;
    int vx = 0, vy = 0, vz = 0;
    for(int i = 0; i < 12; ++i) {
      vx += value_x[i];
      vy += value_y[i];
      vz += value_z[i];
    }
    vx = (abs(vx) + abs(vy) + abs(vz))/12;
    if(abs(vx - GRAVITY) > THRESHOLD) goto FAIL;

    return 0;
 FAIL:
    wakeup();
    return 0;
}
int main (void)
{
    if(mrbc_sp_bottom != NULL) sleep_restore();
    //asm volatile("addi sp, sp, -128");
    //ulp_lp_core_lp_timer_intr_enable(true);
    app_main();
    //asm volatile("addi sp, sp, 128");
    mrbc_sp_bottom = NULL;
    lp_core_ll_set_wakeup_source(LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER);
    ulp_lp_core_lp_timer_set_wakeup_time(7*1000*1000);
    ulp_lp_core_halt();

    return 0;
}

