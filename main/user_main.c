#include <esp_log.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "driver/gpio.h"

static const char *TAG = "main";

#define GPIO_OUTPUT_LED 2
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_LED)
#define GPIO_INPUT_BTN 5
#define GPIO_INPUT_PIN_SEL (1ULL<<GPIO_INPUT_BTN)

static xQueueHandle gpio_evt_queue;

static void gpio_isr_handler(void *arg) {
    const uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task(void *arg) {
    uint32_t io_num;

    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            int level = gpio_get_level(io_num);
            if (level) {
                gpio_set_level(GPIO_OUTPUT_LED, 1);
            } else {
                gpio_set_level(GPIO_OUTPUT_LED, 0);
            }
        }
    }
}

void app_main(void) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    gpio_set_level(GPIO_OUTPUT_LED, 0);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(GPIO_INPUT_BTN, gpio_isr_handler, (void *) GPIO_INPUT_BTN);

    ESP_LOGI(TAG, "Setup");

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
