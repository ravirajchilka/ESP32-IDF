#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define PIN13 (gpio_num_t) 13
#define PIN12 (gpio_num_t) 12
#define PIN14 (gpio_num_t) 14

void app_main(void) {
    
    int level_flag = 0;
    
    esp_rom_gpio_pad_select_gpio(PIN12);
    gpio_set_direction(PIN12, GPIO_MODE_INPUT);
    esp_rom_gpio_pad_select_gpio(PIN13);
    gpio_set_direction(PIN13, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(PIN14);
    gpio_set_direction(PIN14, GPIO_MODE_OUTPUT);

    while (1) {

        int btn_click_level = gpio_get_level(PIN12);
        
        // Turn off LED as soon as release of the button
        gpio_set_level(PIN13,0);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(PIN14,0);
           
        if(btn_click_level) {
            level_flag = !level_flag; 
            printf("flag\n %d",level_flag);
            gpio_set_level(PIN13,level_flag);
            vTaskDelay(pdMS_TO_TICKS(200));
            gpio_set_level(PIN14,!level_flag);
        } 
    }
}