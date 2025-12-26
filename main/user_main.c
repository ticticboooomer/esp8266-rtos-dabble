#include <stdio.h>
#include "driver/gpio.h"


void app_main(void) {
    printf("Running Hello World\n");

    const gpio_num_t led_pin = GPIO_NUM_2;
    const gpio_num_t btn_pin = GPIO_NUM_16;

    const int level = gpio_get_level(btn_pin);
    if (level) {
        gpio_set_level(led_pin, 1);
    } else {
        gpio_set_level(led_pin, 0);
    } 
}
