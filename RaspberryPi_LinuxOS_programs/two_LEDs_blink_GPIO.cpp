#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <chrono>
#include <thread>
#include <vector>

std::uint8_t fd;
struct gpio_v2_line_request req;

void init_GPIO(const char* fd_pathname, const std::vector<std::uint8_t> &offsets) {
    fd = open(fd_pathname, O_RDWR);
    if(fd == -1) {
        std::cerr << "Failed to open the file descriptor ";
        exit(1);
    }

    // Set GPIO pin properties   
    memset(&req, 0, sizeof(req));
    req.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    req.num_lines = offsets.size();
    for (size_t i = 0; i < offsets.size(); i++) {
        req.offsets[i] = offsets[i];
    }
    strncpy(req.consumer, "led_blink", sizeof(req.consumer)-1);

    if(ioctl(fd, GPIO_V2_GET_LINE_IOCTL, &req) == -1) {
        std::cerr << "Failed to get GPIO line";
        close(fd);
        exit(1);
    }
}

void set_GPIO_value(const std::vector<std::uint8_t> &values) {
    struct gpio_v2_line_values data;
    memset(&data, 0, sizeof(data));
    data.mask = (1 << values.size()) - 1; // to make last 2 bits 1, shit by 2bits and 
    //subtract 1 values.size() = 2, 
	// this results in 1 << 2, which is 00000100 in binary. 
	// Subtracting 1 from this result gives 00000100 - 1 = 00000011
    for(size_t i = 0; i< values.size();i++) { // {1,1,1,0} this will be converted to 00001110
	    data.bits = data.bits | values[i] << i;
    }
    if(ioctl(req.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &data) == -1) {
        std::cerr << "Failed to set GPIO values using data object";
    }
}

void close_GPIO_fd() {
    close(fd);
}

int main() {
    auto offsets = std::vector<std::uint8_t> {4, 17};
    init_GPIO("/dev/gpiochip0", offsets);
	
    while(true) {
        set_GPIO_value({1, 1});  // Turn on both LEDs
        //set_GPIO_value({1, 0});  // Turn on 1st LED only
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        set_GPIO_value({0, 0});  // Turn off both LEDs
        //set_GPIO_value({0, 1});  // Turn on 2nd LED only
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }	

    close_GPIO_fd();
    return 0;
}

