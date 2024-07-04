/*
    The US sensor sends echo signals and receives it like Bats to detect the obstacles, we can find the distance of the obstacle by calculating the
    time difference between signal sent and recieved. In this example I have'nt calculated the distance and kept it simple. Watch the video added in
    the same folder 

*/

#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

const char *pathname = "/dev/gpiochip0";
static constexpr auto offsetOutTrigger = std::uint8_t {4};
static constexpr auto offsetInEcho = std::uint8_t {27};
static constexpr auto offsetLED = std::uint8_t{22};
const std::string consumer = "ultrasonic sensor";

int main() {
    
    try {

        gpiod::chip chip(pathname);	
        
        // Set pin numbers
        gpiod::line outLine = chip.get_line(offsetOutTrigger);
        gpiod::line inLine = chip.get_line(offsetInEcho);
	    gpiod::line ledLine = chip.get_line(offsetLED);
    
        // Set direction
        outLine.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
        inLine.request({consumer, gpiod::line_request::DIRECTION_INPUT, 0});
	    ledLine.request({consumer,gpiod::line_request::DIRECTION_OUTPUT,0});
	// outer while loop is for sending signal
	    while(1) {
            std::uint64_t count = 0;
            
            outLine.set_value(1);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            outLine.set_value(0);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
	
	// inner while loop is for processing recieved signal from 1st while loop
	    while(1) {
		 if(inLine.get_value()==1) { // when echo signal goes high start counting
		    count++;	
		 } else if (count > 0) { // once echo signal goes low then print the count, break loop to repeat
		    std::cout<< count<<std::endl;
		    if(count > 10) {
			    ledLine.set_value(0);
		    } else {
			    ledLine.set_value(1);
		    }
		    break;
		 }  

		    std::this_thread::sleep_for(std::chrono::microseconds(10));
	    }

	    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}    

    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;	
    }

    return 0;
}



/* // For exact time calculation //
#include <iostream>
#include <gpiod.hpp>
#include <chrono>
#include <thread>

const char *pathname = "/dev/gpiochip0";
static constexpr auto offsetOutTrigger = std::uint8_t {4};
static constexpr auto offsetInEcho = std::uint8_t {17};
const std::string consumer = "ultrasonic sensor";

// Function to measure pulse width
long long measure_pulse(gpiod::line& line, int state, long long timeout_us) {
    auto start = std::chrono::high_resolution_clock::now();
    while (line.get_value() != state) {
        if (std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count() > timeout_us) {
            return 0; // Timeout
        }
    }
    start = std::chrono::high_resolution_clock::now();
    while (line.get_value() == state) {
        if (std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count() > timeout_us) {
            return 0; // Timeout
        }
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start).count();
}

int main() {
    try {
        gpiod::chip chip(pathname);	
        
        // Set pin numbers
        gpiod::line outLine = chip.get_line(offsetOutTrigger);
        gpiod::line inLine = chip.get_line(offsetInEcho);
    
        // Set direction
        outLine.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
        inLine.request({consumer, gpiod::line_request::DIRECTION_INPUT, 0});

        while(1) {
            // Trigger pulse
            outLine.set_value(1);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            outLine.set_value(0);
            
            // Measure echo pulse width
            long long pulse_width = measure_pulse(inLine, 1, 30000); // 30ms timeout
            
            if (pulse_width > 0) {
                // Calculate distance (speed of sound = 34300 cm/s)
                double distance = pulse_width * 34300.0 / (2 * 1000000);
                
                std::cout << "Pulse width: " << pulse_width << " microseconds" << std::endl;
                std::cout << "Distance: " << distance << " cm" << std::endl;
            } else {
                std::cout << "Measurement timeout" << std::endl;
            }
            
            // Wait before next measurement
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
   
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;	
    }

    return 0;
}
*/
