#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

int main() {
    const char* gpiopathname = "/dev/gpiochip0";
    const unsigned int offset1 = 4;  // GPIO pin number for LED
    const unsigned int offset2 = 27;  // GPIO pin number for button
    const std::string consumer = "Button and LED";

    try {
        // Open GPIO chip
        gpiod::chip chip(gpiopathname);

        // Get line handle
        gpiod::line line1 = chip.get_line(offset1);
        gpiod::line line2 = chip.get_line(offset2);

        // Request line as output for LED
        line1.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});

        // Request line as input for button
        line2.request({consumer, gpiod::line_request::DIRECTION_INPUT, 0});

        std::cout << "Waiting for button click on GPIO " << offset2 << ". Press Ctrl+C to exit." << std::endl;

        while (true) {
            if (line2.get_value() == 1) {
                line1.set_value(1);  // Turn on LED when button is pressed
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else {
                line1.set_value(0);  // Turn off LED when button is not pressed
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

