/* In this code successfully switched from using the older sysfs interface to the more modern GPIO character device interface.
This new approach is more robust and is the preferred method for GPIO control on newer Raspberry Pi systems.
The code now directly interacts with the /dev/gpiochip0 device, which doesn't require manual exporting and unexporting of GPIO pins.
Used ioctl calls to configure and control the GPIO pin, which is more efficient and less prone to timing issues.
*/
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

int fd;
struct gpio_v2_line_request req;

void initGPIO(const char* chipname, unsigned int offset) {
    fd = open(chipname, O_RDWR);
    if (fd < 0) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        exit(1);
    }

    memset(&req, 0, sizeof(req));
    req.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    req.num_lines = 1;
    req.offsets[0] = offset;
    strncpy(req.consumer, "LED_Blink", sizeof(req.consumer) - 1);

    if (ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req) < 0) {
        std::cerr << "Failed to get GPIO line" << std::endl;
        close(fd);
        exit(1);
    }
}

void setValue(int value) {
    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.mask = 1;
    data.bits = value ? 1 : 0;

    if (ioctl(req.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &data) < 0) {
        std::cerr << "Failed to set GPIO value" << std::endl;
    }
}

void closeGPIO() {
    close(fd);
}

int main() {
    // Replace "/dev/gpiochip0" with necessary GPIO chip device
    
    initGPIO("/dev/gpiochip0", 4);

    std::cout << "LED Blinking. Press Ctrl+C to exit." << std::endl;

    while (true) {
        setValue(1);  
        usleep(500000);  
        setValue(0); 
        usleep(500000);  
    }

    closeGPIO();
    return 0;
}


/* using C++ class */
/*
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

class GPIO {
private:
    int fd;
    struct gpio_v2_line_request req;

public:
    GPIO(const char* chipname, unsigned int offset) {
        fd = open(chipname, O_RDWR);
        if (fd < 0) {
            std::cerr << "Failed to open GPIO chip" << std::endl;
            exit(1);
        }

        memset(&req, 0, sizeof(req));
        req.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
        req.num_lines = 1;
        req.offsets[0] = offset;
        strncpy(req.consumer, "LED_Blink", sizeof(req.consumer) - 1);

        if (ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req) < 0) {
            std::cerr << "Failed to get GPIO line" << std::endl;
            close(fd);
            exit(1);
        }
    }

    ~GPIO() {
        close(fd);
    }

    void setValue(int value) {
        struct gpio_v2_line_values data;
        memset(&data, 0, sizeof(data));
        data.mask = 1;
        data.bits = value ? 1 : 0;

        if (ioctl(req.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &data) < 0) {
            std::cerr << "Failed to set GPIO value" << std::endl;
        }
    }
};

int main() {
    GPIO led("/dev/gpiochip0", 4);

    std::cout << "LED Blinking. Press Ctrl+C to exit." << std::endl;

    while (true) {
        led.setValue(1); 
        usleep(500000);  
        led.setValue(0); 
        usleep(500000);  
    }

    return 0;
}


*/

