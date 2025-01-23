idf.py add-dependency "espressif/aht20^1.0.0"


```c
/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "driver/i2c.h"
#include "aht20.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "aht20 test";

#define TEST_MEMORY_LEAK_THRESHOLD (-400)

#define I2C_MASTER_SCL_IO   0   /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO   1   /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM      I2C_NUM_0               /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ  100000                  /*!< I2C master clock frequency */

static aht20_dev_handle_t aht20 = NULL;

/**
 * @brief i2c master initialization
 */
 static void i2c_bus_init(void)
 {
    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &i2c_conf);
    // TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C config returned error");

    ret = i2c_driver_install(I2C_MASTER_NUM, i2c_conf.mode, 0, 0, 0);
    // TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C install returned error");
 }

static void i2c_sensor_ath20_init(void)
{
    aht20_i2c_config_t i2c_conf = {
        .i2c_port = I2C_MASTER_NUM,
        .i2c_addr = AHT20_ADDRRES_0,
    };

    i2c_bus_init();
    aht20_new_sensor(&i2c_conf, &aht20);
    // TEST_ASSERT_NOT_NULL_MESSAGE(aht20, "AHT20 create returned NULL");
}

void app_main(void)
{
    printf("AHT20 TEST \n");
    // unity_run_menu();
    i2c_sensor_ath20_init();
    while(1){
        uint32_t temp_raw, hum_raw;
        float temp, hum;

        aht20_read_temperature_humidity(aht20, &temp_raw, &temp, &hum_raw, &hum);
        ESP_LOGI(TAG, "Humidity      : %2.2f %%", hum);
        ESP_LOGI(TAG, "Temperature   : %2.2f degC", temp);
        ESP_LOGI(TAG, "==============================");
    }
}
```
https://blog.csdn.net/Modest_WANG/article/details/140636904

```
bugu@bugu-desktop:~/MaixCDK/kvm/kvm_system$ cat /etc/logid.cfg 
devices: (
{
    name: "M585/M590 Multi-Device Mouse";
    buttons: (
        {
            cid: 0x52;
            action =
            {
                type: "Keypress";
                keys: ["KEY_LEFTSHIFT"];
            };
        },
        {
            cid: 0x53;
            action =
            {
                type: "Keypress";
                keys: ["KEY_LEFTCTRL","KEY_V"];
            };
        },
        {
            cid: 0x56;
            action =
            {
                type: "Keypress";
                keys: ["KEY_LEFTCTRL","KEY_C"];
            };
        },
        {
            cid: 0x5b;
            action =
            {
                type: "Keypress";
                keys: ["KEY_LEFTCTRL","KEY_LEFTALT","KEY_RIGHT"];
            };
        },
        {
            cid: 0x5d;
            action =
            {
                type: "Keypress";
                keys:["KEY_LEFTCTRL","KEY_LEFTALT","KEY_LEFT"];
            };
        }

    );
    hiresscroll:
    {
        hires: true;
        invert: true;
        target: false;
    };
}
);
```