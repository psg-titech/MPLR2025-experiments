#include <stdio.h>
#include <stdint.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "soc/rtc.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_ulp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_ulp_core_main_bin_end");
#define ADXL367_I2C_ADDR 0x53

static void lp_core_init(void)
{
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));
}

static void lp_i2c_init(void)
{
    /* Initialize LP I2C with default configuration */
    const lp_core_i2c_cfg_t i2c_cfg = {
        .i2c_pin_cfg.sda_io_num = GPIO_NUM_6,
        .i2c_pin_cfg.scl_io_num = GPIO_NUM_7,
        .i2c_pin_cfg.sda_pullup_en = false,
        .i2c_pin_cfg.scl_pullup_en = false,
        .i2c_timing_cfg.clk_speed_hz = 20000,
        LP_I2C_DEFAULT_SRC_CLK()
    };
    ESP_ERROR_CHECK(lp_core_i2c_master_init(LP_I2C_NUM_0, &i2c_cfg));
}
static uint32_t getCpuFrequencyMhz() {
  rtc_cpu_freq_config_t conf;
  rtc_clk_cpu_freq_get_config(&conf);
  return conf.freq_mhz;
}

static void delayUs(int wait) {
  if(wait > 100) {
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
extern int ulp_lp_core_i2c_master_write_to_device(int, uint16_t, const uint8_t *, size_t, int32_t);
extern int ulp_lp_core_i2c_master_read_from_device(int, uint16_t, uint8_t *, size_t, int32_t);


#define NUM 30

#define SHT30_I2C_ADDR 0x44
const uint8_t LOW_REPEAT_READ[] = {0x24, 0x16};

uint32_t tmp[NUM];
uint32_t rh[NUM];

void read_sensor(int index) {
    uint8_t data_rd[6];
    ulp_lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, LOW_REPEAT_READ, sizeof(LOW_REPEAT_READ), -1);
    delayMs(6);
    ulp_lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, SHT30_I2C_ADDR, data_rd, sizeof(data_rd), -1);
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
int app_app_main(void) {
  for(int i = 0; i < NUM; ++i) {
    read_sensor(i);
    delayMs(60*1000-6);
  }
  return 0;
}


void app_main(void)
{
    rtc_gpio_init(1);
    rtc_gpio_set_direction(1, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(1);
    rtc_gpio_pullup_dis(1);
    rtc_gpio_set_level(1, 1);
    lp_i2c_init();
    lp_core_init();
    while(1) {
        rtc_gpio_set_level(1, 0);
        app_app_main();
        rtc_gpio_set_level(1, 1);
    }
}
