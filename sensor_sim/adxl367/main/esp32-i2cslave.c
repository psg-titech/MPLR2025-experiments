/**
 * @file adxl367_simulator.c
 * @brief ESP32 to simulate ADXL367 accelerometer via I2C. (Revised)
 *
 * This program makes an ESP32 act as an I2C slave device, simulating
 * the behavior of an ADXL367 accelerometer.
 *
 * Revision improvements:
 * - Implements a virtual register map to allow both reading from and writing to registers.
 * - The master can now read the POWER_CTL register (0x2D) to check the current mode.
 *
 * It supports:
 * - Reading/Writing the POWER_CTL register (0x2D) to switch modes.
 * - Responding to FIFO read commands (accessing register 0x0E) with fixed
 * acceleration data representing a stationary state (+1G on the Z-axis).
 * - Fixed measurement range of +-2G.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "driver/gpio.h"

// --- I2C Configuration ---
#define I2C_SLAVE_SCL_IO          GPIO_NUM_22  // Select via menuconfig (e.g., GPIO 22)
#define I2C_SLAVE_SDA_IO          GPIO_NUM_21  // Select via menuconfig (e.g., GPIO 21)
#define I2C_SLAVE_NUM             I2C_NUM_0                     // I2C port number
#define I2C_SLAVE_TX_BUF_LEN      256                           // TX buffer size
#define I2C_SLAVE_RX_BUF_LEN      256                           // RX buffer size
#define ESP_SLAVE_ADDR            0x53                          // ADXL367 I2C address

// --- ADXL367 Register Definitions ---
#define ADXL367_REG_FIFO_DATA     0x18
#define ADXL367_REG_POWER_CTL     0x2D

// --- ADXL367 Command Definitions for POWER_CTL ---
#define ADXL367_CMD_STANDBY       0x00
#define ADXL367_CMD_MEASURE       0x02

// --- Simulator State ---
typedef enum {
    MODE_STANDBY,
    MODE_MEASUREMENT
} adxl367_sim_mode_t;

#define INDICATOR_LED GPIO_NUM_23

static const char *TAG = "ADXL367_SIM";
static volatile adxl367_sim_mode_t simulator_mode = MODE_STANDBY;

// --- Virtual Register Map ---
// A simple array to simulate the sensor's internal registers.
static uint8_t adxl367_regs[256];

// Stationary data: X=0, Y=0, Z=+1G.
// At +-2G range, sensitivity is 100 LSB/g. So +1G = 100 (0x0064).
// Data is sent as Little Endian (LSB first).
const uint8_t stationary_fifo_data[6] = {
    0x00, 0x00, // X-axis MSB, LSB
    0x00, 0x00, // Y-axis MSB, LSB
    0x0F, 0xA0  // Z-axis MSB, LSB (4000)
};

static QueueHandle_t event_queue;
// Prepare a callback function
static bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
  static int ledstate = 1;
  BaseType_t xTaskWoken;
  uint16_t v = 0;
  memcpy(&v, evt_data->buffer, sizeof(char) * (evt_data->length >= 2 ? 2 : evt_data->length));
  if(v == 0x022D) {
    gpio_set_level(INDICATOR_LED, ledstate); ledstate = !ledstate;
  }
  xQueueSendFromISR(event_queue, &v, &xTaskWoken);
  return 1;
}

// Prepare a callback function
static bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
  i2c_slave_dev_handle_t handle = (i2c_slave_dev_handle_t)arg;
  uint32_t writelen;
  i2c_slave_write(handle,  stationary_fifo_data, sizeof(stationary_fifo_data), &writelen, 1000);
  return 1;
}

/**
 * @brief Initialize I2C in slave mode
 */
static esp_err_t i2c_slave_init(void) {
    i2c_slave_config_t conf_slave = {
        .i2c_port = I2C_SLAVE_NUM,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = 100,
        .receive_buf_depth = 100,
    };

    i2c_slave_dev_handle_t slave_handle;
    ESP_ERROR_CHECK(i2c_new_slave_device(&conf_slave, &slave_handle));


    // Register callback in a task
    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
        .on_request = i2c_slave_request_cb
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, slave_handle));
    return ESP_OK;
}

/**
 * @brief Main task for the ADXL367 simulator
 */
#if 0
static void adxl367_simulator_task(void *arg) {
    uint8_t *rx_data = (uint8_t *)malloc(I2C_SLAVE_RX_BUF_LEN);
    if (rx_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for RX data");
        vTaskDelete(NULL);
    }
    
    // --- Initialize Virtual Registers ---
    memset(adxl367_regs, 0, sizeof(adxl367_regs));
    adxl367_regs[ADXL367_REG_POWER_CTL] = ADXL367_CMD_STANDBY; // Default to standby mode

    ESP_LOGI(TAG, "ADXL367 Simulator started. Mode: STANDBY");

    while (1) {
        i2c_reset_rx_fifo(I2C_SLAVE_NUM);
        int size = i2c_slave_read_buffer(I2C_SLAVE_NUM, rx_data, I2C_SLAVE_RX_BUF_LEN, pdMS_TO_TICKS(1000));

        if (size > 0) {
            //ESP_LOGI(TAG, "COMING 0x%02X", rx_data[0]);
            printf("0x%02X ", rx_data[0]);
            uint8_t reg_addr = rx_data[0];
            // --- Process Write Commands ---
            if (size > 1) { // Master is writing data to a register
                adxl367_regs[reg_addr] = rx_data[1];
//                ESP_LOGI(TAG, "Master wrote 0x%02X to REG[0x%02X]", rx_data[1], reg_addr);

                // Update internal mode based on POWER_CTL register value
                if (reg_addr == ADXL367_REG_POWER_CTL) {
                    if (adxl367_regs[ADXL367_REG_POWER_CTL] == ADXL367_CMD_MEASURE && simulator_mode != MODE_MEASUREMENT) {
                        simulator_mode = MODE_MEASUREMENT;
//                        ESP_LOGI(TAG, "Mode changed to MEASUREMENT");
                    } else if (adxl367_regs[ADXL367_REG_POWER_CTL] != ADXL367_CMD_MEASURE && simulator_mode != MODE_STANDBY) {
                        // Any value other than MEASURE puts it in standby
                        simulator_mode = MODE_STANDBY;
//                        ESP_LOGI(TAG, "Mode changed to STANDBY");
                    }
                }
            }

            // --- Prepare Read Response ---
            // The master specified a register address, now we prepare the data for the subsequent read.
            i2c_reset_tx_fifo(I2C_SLAVE_NUM);
            
            switch(reg_addr) {
                case ADXL367_REG_FIFO_DATA:
                    if (simulator_mode == MODE_MEASUREMENT) {
                        i2c_slave_write_buffer(I2C_SLAVE_NUM, (uint8_t*)stationary_fifo_data, sizeof(stationary_fifo_data), 0);
//                        ESP_LOGI(TAG, "FIFO data prepared for master (6 bytes)");
                    } else {
                        uint8_t empty_data[6] = {0};
                        i2c_slave_write_buffer(I2C_SLAVE_NUM, empty_data, sizeof(empty_data), 0);
//                        ESP_LOGW(TAG, "FIFO read attempted in STANDBY mode. Returning empty data.");
                    }
                    break;
                
                    /*default:
                    // For any other register, return its value from our virtual map
                    ESP_LOGI(TAG, "Preparing REG[0x%02X] (value: 0x%02X) for master to read", reg_addr, adxl367_regs[reg_addr]);
                    i2c_slave_write_buffer(I2C_SLAVE_NUM, &adxl367_regs[reg_addr], 1, 0);
                    break;*/
            }
        }
    }
    free(rx_data);
    vTaskDelete(NULL);
}
#endif
void app_main(void)
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1 << INDICATOR_LED;
    gpio_config(&io_conf);
    event_queue = xQueueCreate(32, sizeof(uint16_t));
    ESP_ERROR_CHECK(i2c_slave_init());
    ESP_LOGI(TAG, "I2C slave initialized successfully.");
    //xTaskCreate(adxl367_simulator_task, "adxl367_simulator_task", 2048, NULL, 10, NULL);
    uint16_t evt;
    int i = 0;
    while(1) {
      if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        printf("RECV[%d]: 0x%02X 0x%02X\n", i++, evt >> 8, evt & 0xFF);
      }
    }
}
