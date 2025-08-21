#include <stdio.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "soc/rtc.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_ulp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_ulp_core_main_bin_end");

static void lp_core_init(void)
{
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));
}

static void lp_gpio_init(void)
{
    rtc_gpio_init(1);
    rtc_gpio_set_direction(1, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(1);
    rtc_gpio_pullup_dis(1);
    rtc_gpio_set_level(1, 0);
    rtc_gpio_init(0);
    rtc_gpio_set_direction(0, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(0);
    rtc_gpio_pullup_dis(0);
    rtc_gpio_init(6);
    rtc_gpio_set_direction(6, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(6);
    rtc_gpio_pullup_dis(6);
    rtc_gpio_set_level(6, 0);
}
static uint32_t getCpuFrequencyMhz() {
  rtc_cpu_freq_config_t conf;
  rtc_clk_cpu_freq_get_config(&conf);
  return conf.freq_mhz;
}

static void delayUs(int wait) {
  if(wait > 300) {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_sleep_enable_timer_wakeup(wait);
    esp_light_sleep_start();
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    goto FIN;
  }
  if(wait > 1000) {
    vTaskDelay(wait / 1000 / portTICK_PERIOD_MS);
    wait = wait % 1000;
  }
  if(wait == 0) goto FIN;
  uint32_t end = (wait * getCpuFrequencyMhz()) + esp_cpu_get_cycle_count();
  while(esp_cpu_get_cycle_count()< end);
FIN:
  return;
}

static void delayMs(int wait) {
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  esp_sleep_enable_timer_wakeup(1000 * wait);
  esp_light_sleep_start();
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
}

static int pulseIn(int gpio_no, int value, int timeout)  {
  uint32_t mhz = getCpuFrequencyMhz();
  uint32_t end;
  if(timeout == 0) while(value != rtc_gpio_get_level(gpio_no));
  else {
    end = (timeout * mhz) + esp_cpu_get_cycle_count();
    while(esp_cpu_get_cycle_count()< end) {
      if(value == rtc_gpio_get_level(gpio_no)) goto NEXT;
    }
    return 0;
  }
  NEXT:
  uint32_t start = esp_cpu_get_cycle_count();
  if(timeout == 0) {
    while(value == rtc_gpio_get_level(gpio_no));
    goto FIN;
  } else {
    end = (timeout * mhz) + start;
    while(esp_cpu_get_cycle_count()< end) {
      if(value != rtc_gpio_get_level(gpio_no)) goto FIN;
    }
    return 0;
  }
  FIN: return (esp_cpu_get_cycle_count() - start) / mhz;
}

const int RES_LEN = 30;

void sort(int length, int * ary) {
  for(int swapped = 1; swapped;) {
    swapped = 0;
    for(int i = 0; i < length-1; ++i) {
      int aryi = ary[i];
      int aryi1 = ary[i+1];
      if(aryi > aryi1) {
        ary[i] = aryi1;
        ary[i+1] = aryi;
        swapped = 1;
      }
    }
  }
}

int buf[20];
int result[30]; // RES_LEN is not constexpr...

void app_app_main(void) {
  for(int idx = 0; idx < RES_LEN; ++idx) {
    for(int i = 0; i < 20; ++i) {
      rtc_gpio_set_level(6, 1);
      delayUs(10);
      rtc_gpio_set_level(6, 0);
      buf[i] = pulseIn(0, 1, 200000);
      delayMs(2999);
    }
    sort(20, buf);
    int res = 0;
    for(int i = 2; i < 18; ++i) res += buf[i];
    result[idx] = res / 16;
  }
}

void app_main(void)
{
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    lp_gpio_init();
    lp_core_init();
    // AUTO makes the RTC IO unstable.
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    while(1) {
        rtc_gpio_set_level(1, 0);
        app_app_main();
        rtc_gpio_set_level(1, 1);
        for(int i = 0; i < RES_LEN; ++i)
          printf("result [%d]: %d\n", i, result[i]);
        return;
    }
}
