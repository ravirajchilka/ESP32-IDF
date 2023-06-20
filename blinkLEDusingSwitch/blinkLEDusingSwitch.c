#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void) {
    // led 18
    // switch 16
    printf("from main\n");
    gpio_set_direction(GPIO_NUM_16,GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_16,GPIO_PULLUP_ONLY);

    gpio_set_direction(GPIO_NUM_18,GPIO_MODE_OUTPUT);

    while(1) {
        if(gpio_get_level(GPIO_NUM_16)) {
            gpio_set_level(GPIO_NUM_18,0);
        } else {
              gpio_set_level(GPIO_NUM_18,1);
        }
        vTaskDelay(10);
    }

}
