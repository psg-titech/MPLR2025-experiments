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

#define WAITMS 1000

#define SHT30_I2C_ADDR 0x44
const char LOW_REPEAT_READ[] = {0x24, 0x16};

uint32_t tmp;
uint32_t rh;

void wakeup(void) {
    ulp_lp_core_lp_timer_disable();
    ulp_lp_core_wakeup_main_processor();
    ulp_lp_core_halt();
}

void sleep(int ms) {
    ulp_lp_core_lp_timer_set_wakeup_time(1000 * ms);
    // enable interruption
    RV_SET_CSR(mstatus, MSTATUS_MIE);
    RV_SET_CSR(mie, MIE_ALL_INTS_MASK);
    asm volatile("wfi");
    // disable interruption
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    RV_CLEAR_CSR(mie, MIE_ALL_INTS_MASK);
}

void read_sensor(uint32_t * rhv, uint32_t * tmpv) {
    char data_rd[6];
    lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, LOW_REPEAT_READ, sizeof(LOW_REPEAT_READ), -1);
    sleep(6);
    lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, data_rd, sizeof(data_rd), -1);
    int t_ticks = (data_rd[0] << 8) + data_rd[1];
    int rh_ticks = (data_rd[3] << 8) + data_rd[4];
    *tmpv = ((t_ticks * 175)/0xFFFF) - 45;
    int rhv2 = ((rh_ticks * 100)/0xFFFF);
    if(rhv2 > 100) {
        rhv2 = 100;
    } else if(rhv < 0) {
        rhv2 = 0;
    }
    *rhv = rhv2;
}

int main (void)
{
    ulp_lp_core_lp_timer_intr_enable(true);
    uint32_t rhv, tmpv;
    read_sensor(&rh, &tmp);
    while(1) {
        sleep(WAITMS);
        read_sensor(&rhv, &tmpv);
        if(rhv != rh || tmpv != tmp) {
            rh = rhv; tmp = tmpv;
            wakeup();
        }
    }
    return 0;
}
