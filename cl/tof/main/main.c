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

extern int ulp_result[30];
extern int ulp_buf[20];
void app_main(void)
{
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    lp_gpio_init();
    rtc_gpio_set_level(1, 1);
    lp_core_init();
    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
    while(1) {
        rtc_gpio_set_level(1, 0);
        ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));
        esp_light_sleep_start();
        rtc_gpio_set_level(1, 1);
        for(int i = 0; i < 30; ++i) printf("RESULT[%d]: %d\n", i, ulp_result[i]);
        for(int i = 0; i < 20; ++i) printf("BUF[%d]: %d\n", i, ulp_buf[i]);
        vTaskDelay(10);
        return;
    }
}
