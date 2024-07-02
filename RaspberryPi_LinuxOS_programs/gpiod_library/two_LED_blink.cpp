#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

int main() {
    const char* gpiopathname = "/dev/gpiochip0";
    const unsigned int offset1 = 4;  // GPIO pin number
    const unsigned int offset2 = 22;  // GPIO pin number
    const std::string consumer = "LED Blink";

    try {
        // Open GPIO chip
        gpiod::chip chip(gpiopathname);

        // Get line handle
        gpiod::line line1 = chip.get_line(offset1);
        gpiod::line line2 = chip.get_line(offset2);

        // Request line as output
        line1.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
        std::cout << "Blinking LED on GPIO " << offset1 << ". Press Ctrl+C to exit." << std::endl;
	
       	line2.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
        std::cout << "Blinking LED on GPIO " << offset2 << ". Press Ctrl+C to exit." << std::endl;


        while (true) {
            line1.set_value(1);
            line2.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            line1.set_value(0);
            line2.set_value(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

