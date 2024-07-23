#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <gpiod.hpp>

// spi setup
const char* spiDevice = "/dev/spidev0.0";
uint8_t mode = SPI_MODE_0;
uint8_t bits = 8;  // Changed to 8 bits
uint32_t speed = 1000000;  // 1 MHz, adjust as needed

// gpio pins setup
const char* gpiopathname = "/dev/gpiochip0";
const uint8_t offset_yellow = 17;
const uint8_t offset_green = 27;
const uint8_t offset_red = 22;
const auto consumer = std::string("led run");

gpiod::chip chip(gpiopathname);

gpiod::line line_yellow = chip.get_line(offset_yellow);
gpiod::line line_green = chip.get_line(offset_green);
gpiod::line line_red = chip.get_line(offset_red);


int spi_fd;

bool spi_init() {
    spi_fd = open(spiDevice, O_RDWR);
    if (spi_fd < 0) {
        std::cerr << "Error opening SPI device" << std::endl;
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
        std::cerr << "Error setting SPI mode" << std::endl;
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        std::cerr << "Error setting bits per word" << std::endl;
        return false;
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        std::cerr << "Error setting max speed" << std::endl;
        return false;
    }

    return true;
}

void spi_close() {
    if (spi_fd >= 0) {
        close(spi_fd);
    }
}

uint16_t readADC() {
    uint8_t tx[3] = {0x01, 0x80, 0x00};  // Start bit, Single-ended, Channel 0
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = speed,
        .delay_usecs = 0,
        .bits_per_word = bits,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        std::cerr << "Error sending SPI message" << std::endl;
        return 0;
    }

    // Combine the two data bytes into a 10-bit result
    return ((rx[1] & 0x03) << 8) | rx[2];
}

// gpio pin reset for led pins
void setLEDsToZero() {
    line_yellow.set_value(0);
    line_green.set_value(0);
    line_red.set_value(0);
}

int main() {
    if (!spi_init()) {
        return -1;
    }

    // gpio pins direction setup
    line_yellow.request({consumer,gpiod::line_request::DIRECTION_OUTPUT},0);
    line_green.request({consumer,gpiod::line_request::DIRECTION_OUTPUT},0);
    line_red.request({consumer,gpiod::line_request::DIRECTION_OUTPUT},0);

    while(1) {
        uint16_t adcValue = readADC();
        float voltage = (adcValue / 1023.0) * 3.3;  // Assuming 3.3V reference
        std::cout << "ADC Value: " << adcValue << ", Voltage: " << voltage << "V" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Read every second
	    
	try {
	    if(voltage > 2) {
            line_yellow.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();

            line_green.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();
            
            line_red.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();

	    } else if(voltage < 1) {
            line_red.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();

            line_green.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();
            
            line_yellow.set_value(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            setLEDsToZero();

	    } else {
		    setLEDsToZero();
	    }   
	}
	catch(const std::exception& e) {
	    std::cerr << e.what();
	}	

    }

    spi_close();
    
    return 0;
}

