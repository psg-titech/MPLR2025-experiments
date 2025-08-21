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

#define ULP_HALT() ulp_lp_core_halt()
#define WAKEUP_MAIN_PROCESSOR() ulp_lp_core_wakeup_main_processor()
#define GET_CYCLE() RV_READ_CSR(mcycle)
#define LP_CORE_CPU_FREQUENCY_HZ 16000000
#define US_TO_CYCLE(v) ((v) * (LP_CORE_CPU_FREQUENCY_HZ / 1000000))
#define CYCLE_TO_US(v) ((v) / (LP_CORE_CPU_FREQUENCY_HZ / 1000000))
#define GET_GPIO(gpionum) ulp_lp_core_gpio_get_level(gpionum)
#define SET_GPIO(gpionum, value) ulp_lp_core_gpio_set_level(gpionum, value)


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

volatile void * mrbc_sp_bottom = NULL;

void sleep_store(void);
void sleep_restore(void);
void sleep_store2(void) {
    ulp_lp_core_halt();
}
void wakeup(void) {
    ulp_lp_core_lp_timer_disable();
    ulp_lp_core_wakeup_main_processor();
    ulp_lp_core_halt();
}

void delayUs(int i) {
// busy loop version
  //ulp_lp_core_delay_us(i);
  if(i < 50) {
    ulp_lp_core_delay_us(i);
    return;
  }
  ulp_lp_core_lp_timer_set_wakeup_time(i);
  if(i < 1000) {
  // wfi version
    // enable interruption
    RV_SET_CSR(mstatus, MSTATUS_MIE);
    RV_SET_CSR(mie, MIE_ALL_INTS_MASK);
    asm volatile("wfi");
    // disable interruption
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    RV_CLEAR_CSR(mie, MIE_ALL_INTS_MASK);
  } else {
// sleep version
    lp_core_ll_set_wakeup_source(LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER);
    sleep_store();
  }
}

void delayMs(int ms) { delayUs(ms*1000); }

int pulseIn(int gpionum, int value, int timeout) {
  uint32_t start;
  for(int i = 0; i < 2; ++i) {
    int v = value ^ i;
    start = GET_CYCLE();
    if(timeout == 0) {
      while(v != GET_GPIO(gpionum));
    } else {
      uint32_t end = US_TO_CYCLE(timeout) + start - 20;
      while(GET_CYCLE() < end) {
        if(v == GET_GPIO(gpionum)) {
          if(i) goto FOR_BREAK; else goto FOR_CONTINUE;
        }
      }
      return 0;
    }
    FOR_CONTINUE: continue;
  }
FOR_BREAK: return CYCLE_TO_US(GET_CYCLE() - start);
}

int app_main(void);

int main (void)
{
    if(mrbc_sp_bottom != NULL) sleep_restore();
    ulp_lp_core_lp_timer_intr_enable(true);
    app_main();
    mrbc_sp_bottom = NULL;
    wakeup();
    return 0;
}
