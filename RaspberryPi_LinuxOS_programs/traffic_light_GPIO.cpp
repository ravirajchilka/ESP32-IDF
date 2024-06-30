#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <vector>
#include <linux/gpio.h>
#include <sys/ioctl.h>

std::uint16_t fd;
struct gpio_v2_line_request req_line;

void init_GPIO_fd(const char *pathname, std::vector<std::uint8_t> &pins) {
    fd = open(pathname, O_RDWR);
    if(fd == -1) {
        std::cerr << "Cant open fd";
        exit(1);
    }

    memset(&req_line, 0 , sizeof(req_line));
    req_line.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    req_line.num_lines = pins.size();
    strncpy(req_line.consumer,"led_blink", sizeof(req_line.consumer)-1);

    for(size_t i = 0;i<pins.size();i++) {
	    req_line.offsets[i] = pins[i];
    }

    if(ioctl(fd,GPIO_V2_GET_LINE_IOCTL,&req_line) == -1) {
        std::cerr << "Failed to get gpio line";
        close(fd);
        exit(1);
    }
} 

void set_GPIO_value(const std::vector<std::uint8_t> &gpiovalues) {
    struct gpio_v2_line_values gpio_lines;
    memset(&gpio_lines,0,sizeof(gpio_lines));
    gpio_lines.mask = (1 << sizeof(gpiovalues)) - 1;
    for(size_t i = 0 ;i<gpiovalues.size(); i++) {
	    gpio_lines.bits |= gpiovalues[i] << i;
    }

    if(ioctl(req_line.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &gpio_lines) == -1) {
        std::cerr << "Cant set gpio values 0 or 1";
        close(fd);
        exit(1);
    }
}

void close_GPIO_fd() {
    close(fd);
}

int main() {
    auto pins = std::vector<std::uint8_t> {4,17,27};   
    init_GPIO_fd("/dev/gpiochip0",pins);
    
    while(1) {	
        set_GPIO_value({1,0,0});
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        set_GPIO_value({0,1,0});
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        set_GPIO_value({0,0,1});
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    close_GPIO_fd();	
    return 0;	
    
}

