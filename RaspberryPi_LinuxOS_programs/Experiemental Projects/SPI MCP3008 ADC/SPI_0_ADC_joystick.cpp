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
uint8_t bits = 8;  // Changed to 8 bits
uint32_t speed = 1000000;  // 1 MHz, adjust as needed

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

int main() {
    if (!spi_init()) {
        return -1;
    }

    while(1) {
        uint16_t adcValue = readADC();
        float voltage = (adcValue / 1023.0) * 3.3;  // Assuming 3.3V reference
        std::cout << "ADC Value: " << adcValue << ", Voltage: " << voltage << "V" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Read every second
    }

    spi_close();
    
    return 0;
}

