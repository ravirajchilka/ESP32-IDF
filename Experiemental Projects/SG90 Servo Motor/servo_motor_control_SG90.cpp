#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

static constexpr auto offsetpwm = std::uint8_t {4};
const char *pathname = "/dev/gpiochip0";
const auto consumer = std::string("servo sg90");

int main() {

    try {
        gpiod::chip chip(pathname);	
        gpiod::line pwmpin = chip.get_line(offsetpwm);
        pwmpin.request({consumer,gpiod::line_request::DIRECTION_OUTPUT});

        while(1) {
            auto count = std::uint64_t {0};
            while(1) {
                count++;
                if(count < 700) {
                    pwmpin.set_value(1);
                    std::this_thread::sleep_for(std::chrono::microseconds(500)); 
                    pwmpin.set_value(0);
                } else if(count < 1400) {
                    pwmpin.set_value(1);
                    std::this_thread::sleep_for(std::chrono::microseconds(1500));
                    pwmpin.set_value(0);
                } else {
                    pwmpin.set_value(1);
                    std::this_thread::sleep_for(std::chrono::microseconds(2500));
                    pwmpin.set_value(0);
                }
                if(count >= 2100) {
                    count = 0;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(500));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

    } catch(std::exception& e) {
	    std::cout << e.what();
    }

    return 0;


}



