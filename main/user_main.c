#include <esp_log.h>

#include "driver/gpio.h"



static const char *TAG = "main";

#define GPIO_OUTPUT_LED 2
#define GPIO_OUTPUT_PIN_SEL (1<<GPIO_OUTPUT_LED)
#define GPIO_INPUT_BTN 16
#define GPIO_INPUT_PIN_SEL (1<<GPIO_INPUT_BTN)

static void gpio_isr_handler(void* arg) {
    const uint32_t gpio_num = (uint32_t)arg;
    const int level = gpio_get_level(gpio_num);
    if (level) {
        gpio_set_level(GPIO_OUTPUT_LED, 1);
    }

    ESP_LOGI(TAG, "GPIO 0 isr triggered");
}

void app_main(void) {

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;

    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(GPIO_INPUT_BTN, gpio_isr_handler, (void*)GPIO_INPUT_BTN);

    ESP_LOGI(TAG, "Setup");

    while (1) {
    }
}
