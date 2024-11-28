#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>
#include <stdexcept>

class LEDblinkTest 
{
private:
    const char* gpiopathname = "/dev/gpiochip0";
    const unsigned int offset = 4;  // GPIO pin number
    const std::string consumer = "LED Blink";
    gpiod::chip chip;
    gpiod::line line;

public:
    LEDblinkTest() : chip(gpiopathname)
	{
		try
		{
			line = chip.get_line(offset);
			line.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error initializing GPIO: " << e.what() << std::endl;
			throw;
		}
	}

	~LEDblinkTest() {
		try
		{
			if (line.is_requested()) {
				line.release();
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error releasing GPIO: " << e.what() << std::endl;
		}
	}

	void blinkOnce(int onTime = 100, int offTime = 300)
	{
		try
		{
			line.set_value(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(onTime));
			line.set_value(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(offTime));
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error blinking LED: " << e.what() << std::endl;
		}
	}

	void blinkContinuously() 
    {
		std::cout << "Blinking LED on GPIO " << offset << ". Press Ctrl+C to exit." << std::endl;
		while (true) {
			blinkOnce();
		}
    }
};

int main() 
{
    try {
        LEDblinkTest ledBlinker;
        ledBlinker.blinkContinuously();
    } catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
