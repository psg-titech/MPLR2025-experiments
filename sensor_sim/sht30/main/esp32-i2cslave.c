#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <time.h>   // time() Für srand
#include <stdlib.h> // rand(), srand()
#include "driver/gpio.h"

// I2C スレーブ設定
#define I2C_SLAVE_SCL_IO GPIO_NUM_22      // ESP32 SCLピン (適宜変更してください)
#define I2C_SLAVE_SDA_IO GPIO_NUM_21      // ESP32 SDAピン (適宜変更してください)
#define I2C_SLAVE_NUM I2C_NUM_0           // ESP32 I2Cポート番号
#define I2C_SLAVE_TX_BUF_LEN 256          // I2Cスレーブ送信バッファ長
#define I2C_SLAVE_RX_BUF_LEN 256          // I2Cスレーブ受信バッファ長
#define ESP_SLAVE_ADDR 0x44               // SHT40 I2Cアドレス

// SHT30 コマンド
#define SHT30_CMD_MEAS_LOW_CS_MSB 0x24
#define SHT30_CMD_MEAS_LOW_CS_LSB 0x16

#define INDICATOR_LED GPIO_NUM_23

static const char *TAG = "sht30_sim_slave";

uint8_t calculate_dummy_crc8(const uint8_t *data, int len) {
    return 0xBD;
}

/**
 * @brief シミュレートされたSHT30センサーデータを生成します。
 * @param data_buf 生成されたデータ（6バイト）を格納するバッファ：
 * [T_MSB, T_LSB, T_CRC, RH_MSB, RH_LSB, RH_CRC]
 */
void generate_sht30_data(uint8_t *data_buf) {
    // 気温をシミュレート: 20.0℃ ～ 30.0℃
    float temperature = 20.0f;// + (float)rand() / (float)(RAND_MAX / 10.0f);
    // 湿度をシミュレート: 40.0%RH ～ 60.0%RH
    float humidity = 40.0f;// + (float)rand() / (float)(RAND_MAX / 20.0f);

    //ESP_LOGI(TAG, "生成データ: 気温 = %.2f C, 湿度 = %.2f %%RH", temperature, humidity);

    // 気温をSHT30の生データ形式に変換 (SHT40と同じ式)
    // T [°C] = -45 + 175 * ST / (2^16 - 1)  =>  ST = (T + 45) / 175 * (2^16 - 1)
    uint16_t s_t_raw = (uint16_t)(((temperature + 45.0f) / 175.0f) * 65535.0f);

    // 湿度をSHT30の生データ形式に変換 (SHT40と異なる式)
    // RH [%] = 100 * SRH / (2^16 - 1)  =>  SRH = RH / 100 * (2^16 - 1)
    if (humidity > 100.0f) humidity = 100.0f; // 念のためクランプ
    if (humidity < 0.0f) humidity = 0.0f;
    uint16_t s_rh_raw = (uint16_t)((humidity / 100.0f) * 65535.0f);

    data_buf[0] = (s_t_raw >> 8) & 0xFF; // 気温 MSB
    data_buf[1] = s_t_raw & 0xFF;        // 気温 LSB
    uint8_t temp_for_crc[] = {data_buf[0], data_buf[1]};
    data_buf[2] = calculate_dummy_crc8(temp_for_crc, 2); // 気温 CRC (ダミー)

    data_buf[3] = (s_rh_raw >> 8) & 0xFF; // 湿度 MSB
    data_buf[4] = s_rh_raw & 0xFF;        // 湿度 LSB
    uint8_t hum_for_crc[] = {data_buf[3], data_buf[4]};
    data_buf[5] = calculate_dummy_crc8(hum_for_crc, 2);  // 湿度 CRC (ダミー)

    ESP_LOGD(TAG, "送信準備データ (raw): T_MSB=0x%02X, T_LSB=0x%02X, T_CRC=0x%02X, RH_MSB=0x%02X, RH_LSB=0x%02X, RH_CRC=0x%02X",
             data_buf[0], data_buf[1], data_buf[2], data_buf[3], data_buf[4], data_buf[5]);
}

/**
 * @brief I2Cスレーブを初期化します。
 */
static esp_err_t i2c_slave_init(void) {
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = ESP_SLAVE_ADDR,
        .clk_flags = 0, // SCLストレッチングのためのオプションフラグ (0でデフォルト)
    };
    esp_err_t err = i2c_param_config(I2C_SLAVE_NUM, &conf_slave);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2Cスレーブ設定失敗 (i2c_param_config): %s", esp_err_to_name(err));
        return err;
    }
    err = i2c_driver_install(I2C_SLAVE_NUM, conf_slave.mode,
                             I2C_SLAVE_RX_BUF_LEN,
                             I2C_SLAVE_TX_BUF_LEN,
                             0); // 割り込みフラグ
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2Cスレーブドライバインストール失敗 (i2c_driver_install): %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "I2Cスレーブ初期化完了。アドレス 0x%02X で待機中...", ESP_SLAVE_ADDR);
    return ESP_OK;
}

/**
 * @brief I2Cスレーブ通信を処理するタスク。
 */
void i2c_slave_task(void *arg) {
    (void)arg; // 未使用引数
    uint8_t rx_data[I2C_SLAVE_RX_BUF_LEN]; // 受信コマンド用バッファ
    uint8_t tx_sensor_data[6];             // 送信センサーデータ用バッファ

    // 乱数ジェネレータのシードを設定
    srand(time(NULL));

    while (1) {
        // マスターからのコマンド（2バイト）を待機
        int rx_size = i2c_slave_read_buffer(I2C_SLAVE_NUM, rx_data, 2, pdMS_TO_TICKS(1000));

        if (rx_size == 2) { // 2バイト受信成功
            ESP_LOGI(TAG, "受信コマンド: 0x%02X%02X", rx_data[0], rx_data[1]);

            // SHT30の低精度読み取りコマンドかチェック
            if (rx_data[0] == SHT30_CMD_MEAS_LOW_CS_MSB &&
                rx_data[1] == SHT30_CMD_MEAS_LOW_CS_LSB) {
                static int ledstate = 1;
                gpio_set_level(INDICATOR_LED, ledstate); ledstate = !ledstate;

                ESP_LOGI(TAG, "SHT30 低精度読み取りコマンド (0x%02X%02X) 受信。データ生成および送信準備。",
                         SHT30_CMD_MEAS_LOW_CS_MSB, SHT30_CMD_MEAS_LOW_CS_LSB);

                // 新しいセンサーデータを生成
                generate_sht30_data(tx_sensor_data);

                // 生成したデータをI2C送信バッファにロード
                int written_size = i2c_slave_write_buffer(I2C_SLAVE_NUM, tx_sensor_data, sizeof(tx_sensor_data), pdMS_TO_TICKS(500));
                if (written_size == sizeof(tx_sensor_data)) {
                    ESP_LOGI(TAG, "%d バイトのデータをマスターの読み取り用に準備しました。", written_size);
                } else {
                    ESP_LOGW(TAG, "送信バッファへのデータ書き込み失敗。書き込みサイズ: %d バイト (期待: %u バイト)", written_size, sizeof(tx_sensor_data));
                }
            } else {
                ESP_LOGW(TAG, "未知のコマンドを受信: 0x%02X%02X", rx_data[0], rx_data[1]);
            }
        } else if (rx_size > 0) { // 期待しないバイト数を受信した場合
            ESP_LOGW(TAG, "期待しないバイト数 (%d バイト) を受信しました。", rx_size);
        }
        // タイムアウト(rx_size=0)やエラー(rx_size<0)の場合は何もしないか、ログ出力のみ

        // 他のタスクにCPUを譲るための短い遅延
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


void app_main(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1 << INDICATOR_LED;
    gpio_config(&io_conf);
    ESP_LOGI(TAG, "SHT30 I2C Slave Simulator is starting...");
    ESP_ERROR_CHECK(i2c_slave_init());

    // I2Cスレーブ処理タスクを作成・実行
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 2048 * 2, NULL, 10, NULL);
}
