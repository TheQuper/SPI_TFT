#ifndef ILI9341_SPI_H
#define ILI9341_SPI_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Board pin identifiers. Set these to the pin numbering used by your
 * ATSAMD21 board. The platform functions below configure the SERCOM SPI
 * peripheral and GPIO pin mux for these pins.
 */
#ifndef ILI9341_MOSI_PIN
#define ILI9341_MOSI_PIN 0u
#endif
#ifndef ILI9341_SCK_PIN
#define ILI9341_SCK_PIN 0u
#endif
#ifndef ILI9341_CS_PIN
#define ILI9341_CS_PIN 0u
#endif
#ifndef ILI9341_DC_PIN
#define ILI9341_DC_PIN 0u
#endif
#ifndef ILI9341_RST_PIN
#define ILI9341_RST_PIN 0u
#endif
#ifndef ILI9341_USE_RESET
#define ILI9341_USE_RESET 1
#endif

#define ILI9341_WIDTH  240u
#define ILI9341_HEIGHT 320u

/*
 * Implement these four small board functions in your application. SPI must
 * be configured as 8-bit, MSB first, mode 0, with a clock no faster than
 * 10 MHz. Pin numbers are passed to PlatformInit for SERCOM/pin-mux setup.
 */
void ILI9341_PlatformInit(uint8_t mosi_pin, uint8_t sck_pin,
                          uint8_t cs_pin, uint8_t dc_pin, uint8_t reset_pin);
uint8_t ILI9341_PlatformSPITransfer(uint8_t data);
void ILI9341_PlatformPinWrite(uint8_t pin, bool high);
void ILI9341_PlatformDelayMs(uint16_t milliseconds);

void ILI9341_Init(void);
void ILI9341_SetRotation(uint8_t rotation);
void ILI9341_InvertDisplay(bool invert);
uint16_t ILI9341_GetWidth(void);
uint16_t ILI9341_GetHeight(void);

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color);
void ILI9341_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color);
void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void ILI9341_FillScreen(uint16_t color);

#endif
