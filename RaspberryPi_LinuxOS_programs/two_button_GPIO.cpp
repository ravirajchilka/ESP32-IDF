#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <vector>
#include <string.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

uint8_t fd;
struct gpio_v2_line_request reqOutput, reqInput;

void init_GPIO(const char *pathname,std::uint8_t input_17,std::uint8_t input_27,std::uint8_t output_22,std::uint8_t output_4) {
    fd = open(pathname,O_RDWR);
    if(fd == -1) {
	std::cerr<< "cant open fd";
	exit(1);
    }
    //reqoutput
    memset(&reqOutput,0,sizeof(reqOutput));
    reqOutput.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    reqOutput.offsets[0] = output_22;
    reqOutput.offsets[1] = output_4;
    reqOutput.num_lines = 2;
    strncpy(reqOutput.consumer,"output pins", sizeof(reqOutput.consumer)-1);

    if(ioctl(fd,GPIO_V2_GET_LINE_IOCTL,&reqOutput) == -1) {
	std::cerr<< "cant intialize output";
	exit(1);
	close(fd);
    }

    memset(&reqInput,0,sizeof(reqInput));
    reqInput.config.flags = GPIO_V2_LINE_FLAG_INPUT;
    reqInput.offsets[0] = input_17;
    reqInput.offsets[1] = input_27;
    reqInput.num_lines = 2;
    strncpy(reqInput.consumer,"input pins", sizeof(reqInput.consumer)-1);

    if(ioctl(fd,GPIO_V2_GET_LINE_IOCTL,&reqInput) == -1) {
	std::cerr<< "cant intialize input";
	exit(1);
	close(fd);
    }
}


void write_GPIO_value(std::uint8_t value) {
    struct gpio_v2_line_values line_value;
    line_value.mask = 3;

    if(value) {
	line_value.bits = 3;
	if(ioctl(reqOutput.fd, GPIO_V2_LINE_SET_VALUES_IOCTL,&line_value) == -1) {
	    std::cerr<< "cant write gpio pin";
	    close(fd);
	    exit(1);	
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
 
	line_value.bits = 0; 
        if(ioctl(reqOutput.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &line_value) == -1) {
            std::cerr << "Failed to set GPIO value using data object"; 
	    close(fd);
	    exit(1);	
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    } else {
	line_value.bits = 0;
	if(ioctl(reqOutput.fd, GPIO_V2_LINE_SET_VALUES_IOCTL,&line_value) == -1) {
	    std::cerr<< "cant write gpio pin";
	    close(fd);
	    exit(1);	
	}

    }

}

std::uint8_t read_GPIO_value() {
    struct gpio_v2_line_values line_value;
    line_value.mask = 3;

    if(ioctl(reqInput.fd,GPIO_V2_LINE_GET_VALUES_IOCTL,&line_value) == -1) {
	std::cerr<< "cant read gpio pin";
	close(fd);
	exit(1);	
    }
    return line_value.mask & line_value.bits;
}


int main() {
    init_GPIO("/dev/gpiochip0",17,27,22,4);

    while(1) {
	auto button_state = read_GPIO_value();
	write_GPIO_value(button_state);
    };

    close(fd);
    return 0;
}

