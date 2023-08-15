#include<stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/gpio.h>

#define INPIN_12 (gpio_num_t) 12
#define OUTPIN_13 (gpio_num_t) 13

void app_main(void) {
    int count = 0;
    esp_rom_gpio_pad_select_gpio(INPIN_12);
    gpio_set_direction(INPIN_12,GPIO_MODE_INPUT);
    esp_rom_gpio_pad_select_gpio(OUTPIN_13);
    gpio_set_direction(OUTPIN_13,GPIO_MODE_OUTPUT);

    while(1) {
        int get_btn_level = gpio_get_level(INPIN_12);
        if(get_btn_level) {
            count++;
            printf("Count value %d\n",count);
            if(count>5) {
                gpio_set_level(OUTPIN_13,1);
            }
        } else {
            gpio_set_level(OUTPIN_13,0);
            count = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
