#include <stdio.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "lp_core_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "soc/rtc.h"
#define USE_PWM 1
#if USE_PWM
#include "driver/ledc.h"
#define BREATHING_LED_GPIO      (6)
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY          (2000)
#define FADE_TIME_MS            (500)
#endif

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
    rtc_gpio_pulldown_en(0);
    rtc_gpio_pullup_dis(0);
#if USE_PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = BREATHING_LED_GPIO,
        .sleep_mode     = LEDC_SLEEP_MODE_KEEP_ALIVE,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_ERROR_CHECK(ledc_fade_func_install(0));
#else
    rtc_gpio_init(6);
    rtc_gpio_set_direction(6, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_pulldown_dis(6);
    rtc_gpio_pullup_dis(6);
    rtc_gpio_set_level(6, 0);
#endif
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

const int MAX_ELAPSED = 500;

void app_app_main(void) {
    int prevPress = 0, press = 0;
    while(!prevPress || press) {
#if USE_PWM
      ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, (1 << LEDC_DUTY_RES) - 1, FADE_TIME_MS));
      ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
      delayMs(FADE_TIME_MS);
      ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, 0, FADE_TIME_MS));
      ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT));
      delayMs(FADE_TIME_MS);
#else
      for(int j = -MAX_ELAPSED; j <= MAX_ELAPSED; ++j) {
        int elapsed = abs(j);
        for(int i = 0; i < 2; ++i) {
          rtc_gpio_set_level(6, 0);
          delayUs(elapsed);
          rtc_gpio_set_level(6, 1);
          delayUs(MAX_ELAPSED - elapsed);
        }
      }
#endif
      prevPress = press;
      press = rtc_gpio_get_level(0);
    }
}

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
        app_app_main();
        rtc_gpio_set_level(1, 1);
    }
}
