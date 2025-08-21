#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

// 使用するGPIOピン
#define ECHO_PIN GPIO_NUM_25
#define TRIG_PIN GPIO_NUM_26

// 10cmの距離をシミュレートする時間（マイクロ秒）
#define SIMULATED_DISTANCE_TIME_US 583

static QueueHandle_t event_queue;
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    esp_rom_delay_us(SIMULATED_DISTANCE_TIME_US);
    gpio_set_level(ECHO_PIN, 1);
    esp_rom_delay_us(SIMULATED_DISTANCE_TIME_US);
    gpio_set_level(ECHO_PIN, 0);
    BaseType_t xTaskWoken;
    uint16_t v = 0;
    xQueueSendFromISR(event_queue, &v, &xTaskWoken);
}

void app_main(void) {
    // GPIOの初期設定
    gpio_config_t io_conf_echo = {};
    io_conf_echo.intr_type = GPIO_INTR_DISABLE;
    io_conf_echo.mode = GPIO_MODE_OUTPUT;
    io_conf_echo.pin_bit_mask = (1ULL << ECHO_PIN);
    gpio_config(&io_conf_echo);

    gpio_config_t io_conf_trig = {};
    io_conf_trig.intr_type = GPIO_INTR_NEGEDGE;
    io_conf_trig.mode = GPIO_MODE_INPUT;
    io_conf_trig.pin_bit_mask = (1ULL << TRIG_PIN);
    gpio_config(&io_conf_trig);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(TRIG_PIN, gpio_isr_handler, (void*) TRIG_PIN);
    gpio_set_level(ECHO_PIN, 0);

    event_queue = xQueueCreate(32, sizeof(uint16_t));
    uint16_t evt;
    for(int i = 0; 1; ++i) {
      if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        printf("[%d] RECEIVED\n", i);
      }
    }
}
