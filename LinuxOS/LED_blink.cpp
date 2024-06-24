#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
#include <stdexcept>
#include <system_error>
#include <cstring>

int gpio_fd;
int line_fd;
int pin = 4;  // GPIO pin number and physical pin no. 7

void openGPIO() {
    gpio_fd = open("/dev/gpiochip0", O_RDWR);
    if (gpio_fd < 0) {
        throw std::system_error(errno, std::system_category(), "Failed to open GPIO device");
    }

    struct gpiohandle_request req;
    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req.lines = 1;
    req.lineoffsets[0] = pin;
    strcpy(req.consumer_label, "LED_BLINK");

    if (ioctl(gpio_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0) {
        close(gpio_fd);
        throw std::system_error(errno, std::system_category(), "Failed to get line handle");
    }

    line_fd = req.fd;
}

void closeGPIO() {
    close(line_fd);
    close(gpio_fd);
}

void setGPIOValue(int value) {
    struct gpiohandle_data data;
    data.values[0] = value;
    if (ioctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        throw std::system_error(errno, std::system_category(), "Failed to set GPIO value");
    }
}

int main() {
    try {
        openGPIO();

        std::cout << "Blinking LED. Press Ctrl+C to exit." << std::endl;

        while (true) {
            setGPIOValue(1);  
            sleep(1);         
            setGPIOValue(0);  
            sleep(1);        
        }

        closeGPIO();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
