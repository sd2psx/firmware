#pragma once

#define PIN_BTN_LEFT 23
#define PIN_BTN_RIGHT 21

#define UART_TX 8
#define UART_RX 9
#define UART_PERIPH uart1
#define UART_BAUD 3000000

#define OLED_I2C_SDA 28
#define OLED_I2C_SCL 25
#define OLED_I2C_ADDR 0x3C
#define OLED_I2C_PERIPH i2c0
#define OLED_I2C_CLOCK 400000

#define PSRAM_CS 2
#define PSRAM_CLK 3
#define PSRAM_DAT 4  /* IO0-IO3 must be sequential! */
#define PSRAM_CLKDIV 2

#define SD_PERIPH spi1
#define SD_MISO 24
#define SD_MOSI 27
#define SD_SCK 26
#define SD_CS 29
#define SD_BAUD 45 * 1000000

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define DEBOUNCE_MS 5
