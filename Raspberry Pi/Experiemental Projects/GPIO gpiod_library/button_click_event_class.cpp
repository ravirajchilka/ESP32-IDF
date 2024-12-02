#include<iostream>
#include<gpiod.hpp>
#include<chrono>
#include<thread>
#include<stdexcept>

class BlinkLEDbuttonClick
{
    private:
        const char* gpiopathname = "/dev/gpiochip0";
        const std::uint8_t led_offset = 4;
        const std::uint8_t button_offset = 17;
        const std::string consumer = "led blink on btn click";
        gpiod::chip chip;
        gpiod::line led_line;
        gpiod::line btn_line;


    public:
        BlinkLEDbuttonClick():chip(gpiopathname)
        {
            try {
                led_line = chip.get_line(led_offset);
                btn_line = chip.get_line(button_offset);
                led_line.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});
            	btn_line.request({consumer, gpiod::line_request::DIRECTION_INPUT, 0});
				}
            catch(const std::exception& e) {
                std::cerr << "Error in gpiod allocation" << e.what() << std::endl;
            }
        }
        
        ~BlinkLEDbuttonClick()
        {
            try {
                if(led_line.is_requested() || btn_line.is_requested()) {
                    led_line.release();
                    btn_line.release();
                }
            }
            catch(const std::exception& e) {
                std::cerr << "Error in gpiod release" << e.what() << std::endl;
            }
        }

        void blinkLEDonBtn(std::uint16_t onTime, std::uint16_t offTime)
        {
           while(1) {
						 if (btn_line.get_value() == 1) {
							 led_line.set_value(1);
							 std::this_thread::sleep_for(std::chrono::milliseconds(onTime));
							 led_line.set_value(0);
							 std::this_thread::sleep_for(std::chrono::milliseconds(offTime));
						 }
						 else {
							 led_line.set_value(0);
							 std::this_thread::sleep_for(std::chrono::milliseconds(100));
						 }
				}
        }
};


int main()
{
    BlinkLEDbuttonClick blinkledBtnClick;
    blinkledBtnClick.blinkLEDonBtn(200,200);
    return 0;
}
















