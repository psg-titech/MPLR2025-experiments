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

#define NUM 30

#define SHT30_I2C_ADDR 0x44
const char LOW_REPEAT_READ[] = {0x24, 0x16};

uint32_t tmp[NUM];
uint32_t rh[NUM];

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

void read_sensor(int index) {
    char data_rd[6];
    lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, LOW_REPEAT_READ, sizeof(LOW_REPEAT_READ), -1);
    sleep(6);
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
    read_sensor(counter);
    counter++;
    if(counter == NUM)
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
    ulp_lp_core_lp_timer_set_wakeup_time((60*1000-6)*1000);
    ulp_lp_core_halt();
    return 0;
}
