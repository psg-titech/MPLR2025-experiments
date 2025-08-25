#include <stdio.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_ulp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_ulp_core_main_bin_end");

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


extern int32_t ulp_value_x[20];
extern int32_t ulp_value_y[20];
extern int32_t ulp_value_z[20];
void app_main(void)
{
    rtc_gpio_init(1);
    rtc_gpio_set_direction(1, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(1);
    rtc_gpio_pullup_dis(1);
    rtc_gpio_set_level(1, 1);
    ulp_lp_core_cfg_t cfg = {
      .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU
      //.wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, // LP core will be woken up periodically by LP timer
      //.lp_timer_sleep_duration_us = (7*1000-100)*1000,
    };
    lp_i2c_init();
    lp_core_init();
    while(1) {
        ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
        rtc_gpio_set_level(1, 0);
        ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));
        esp_light_sleep_start();
        rtc_gpio_set_level(1, 1);
        for(int i = 0; i < 12; ++i) printf("%ld %ld %ld\n", ulp_value_x[i], ulp_value_y[i], ulp_value_z[i]);
        vTaskDelay(10);
        return;
    }
}
