#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <vector>
#include <linux/gpio.h>
#include <sys/ioctl.h>

//gpio 17 as input from button
//gpio 27 as input from button
// gpio 22 yellow led positive
// gpio 4 red led positive

std::uint8_t fd;
struct gpio_v2_line_request req_output, req_input;

void init_GPIO(const char *pathname, std::uint8_t output_pin, std::uint8_t input_pin) {
    fd = open(pathname, O_RDWR);
    if(fd == -1) {
        std::cerr << "Can't open fd";
        exit(1);
    }

    memset(&req_output, 0, sizeof(req_output));
    req_output.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    req_output.num_lines = 1;
    req_output.offsets[0] = output_pin;
    strncpy(req_output.consumer, "led pin output", sizeof(req_output.consumer) - 1);
    
    if(ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req_output) == -1) {
        std::cerr << "Can't set gpio pin to output";
        close(fd);
        exit(1);
    }

    memset(&req_input, 0, sizeof(req_input));
    req_input.config.flags = GPIO_V2_LINE_FLAG_INPUT;
    req_input.num_lines = 1;
    req_input.offsets[0] = input_pin;
    strncpy(req_input.consumer, "input pin for button", sizeof(req_input.consumer) - 1);

    if(ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req_input) == -1) {
        std::cerr << "Can't set gpio pin as input";
        close(fd);
        exit(1);
    } 
}

void set_GPIO_value(std::uint8_t value) {
    struct gpio_v2_line_values gpio;
    gpio.mask = 1;
    // led blink on button click simulation
    if(value) {
        gpio.bits = 1;  
        // GPIO_V2_LINE_SET_VALUES_IOCTL sets the value
        if(ioctl(req_output.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &gpio) == -1) {
            std::cerr << "Failed to set GPIO value using data object";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        gpio.bits = 0; 
        if(ioctl(req_output.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &gpio) == -1) {
            std::cerr << "Failed to set GPIO value using data object";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
   // gpio.bits = value ? 1 : 0;
   // if(ioctl(req_output.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &gpio) == -1) {
   //   std::cerr << "Failed to set GPIO value using data object";
   // }
}

std::uint8_t read_GPIO_value() {
    struct gpio_v2_line_values gpio;
    gpio.mask = 1;

    //here GPIO_V2_LINE_GET_VALUES_IOCTL return value in the form of gpio.bits
    if(ioctl(req_input.fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &gpio) == -1) {
        std::cerr << "Failed to get GPIO value using data object";
        return 0;
    }
   
    return gpio.mask & gpio.bits;
}

int main() {
    init_GPIO("/dev/gpiochip0", 4, 17);

    while(true) {
        std::uint8_t button_state = read_GPIO_value();
        set_GPIO_value(button_state);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Debounce delay
    }

    close(fd);
    return 0;

}








