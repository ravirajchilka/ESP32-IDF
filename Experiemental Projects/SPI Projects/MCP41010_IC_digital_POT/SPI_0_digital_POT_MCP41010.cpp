/*
    MCP41010 is digital potentiometer, it uses SPI interface for communication with microcontroller or SBC.
    Watch the video which is in the same folder.
*/

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <chrono>
#include <thread>

const char* spiDevice = "/dev/spidev0.0";
uint8_t mode = SPI_MODE_0;
uint8_t bits = 8;
uint32_t speed = 1000000; 

std::uint8_t spi_fd;

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

void setWiper(uint8_t value) {
    uint8_t tx[2] = {0x00, value};  // Command byte 0x00 for write to wiper
    uint8_t rx[2] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 2,
        .speed_hz = speed,
        .delay_usecs = 0,
        .bits_per_word = bits,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        std::cerr << "Error sending SPI message" << std::endl;
    }
}

int main() {
    if (!spi_init()) {
        return -1;
    }

    // Intial value of potentiometer
    setWiper(0);
   
    // continously increase and decrease the LED brightness
    while(1) {
        for (std::uint8_t i = 0; i <= 210; i += 7) { 
            setWiper(i);
            std::cout << "Wiper set to: " << static_cast<int>(i) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        for (std::uint8_t k = 210; k > 0; k -= 7) {
            setWiper(i);
            std::cout << "Wiper set to: " << static_cast<int>(k) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    spi_close();
    
    return 0;
}

