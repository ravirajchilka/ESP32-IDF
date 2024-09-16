/*gpiod.hpp provides wrapper methods for direct GPIO manipulation, to install the library use following commands
    sudo apt-get update
    sudo apt-get install libgpiod-dev

    to compile use the following commands
    g++ -o your_program your_program.cpp -lgpiod -lgpiodcxx
*/

#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

int main() {
    const char* gpiopathname = "/dev/gpiochip0";
    const unsigned int offset = 4;  // GPIO pin number
    const std::string consumer = "LED Blink";

    try {
        // Open GPIO chip
        gpiod::chip chip(gpiopathname);

        // Get line handle
        gpiod::line line = chip.get_line(offset);

        // Request line as output
        line.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});

        std::cout << "Blinking LED on GPIO " << offset << ". Press Ctrl+C to exit." << std::endl;

        while (true) {
            line.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            line.set_value(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

