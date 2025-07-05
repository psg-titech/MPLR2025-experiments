#include <stdio.h>
#include "esp_sleep.h"
#include "lp_core_i2c.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_ulp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_ulp_core_main_bin_end");

extern uint32_t ulp_tmp[30];
extern uint32_t ulp_rh[30];

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

extern int ulp_counter;
void app_main(void)
{
    rtc_gpio_init(4);
    rtc_gpio_set_direction(4, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(4);
    rtc_gpio_pullup_dis(4);
    rtc_gpio_set_level(4, 1);
    ulp_lp_core_cfg_t cfg = {
      .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU
    };
    lp_i2c_init();
    lp_core_init();
    while(1) {
        ulp_counter = 0;
        ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
        rtc_gpio_set_level(4, 0);
        ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));
        esp_light_sleep_start();
        rtc_gpio_set_level(4, 1);
        for(int i = 0; i < 30; ++i) {
            printf("tmp:%ld rh:%ld\n", ulp_tmp[i], ulp_rh[i]);
        }vTaskDelay(1);
        return;
    }
}
