#ifndef SW_COMPONENTS_UTILITIES_H_
#define SW_COMPONENTS_UTILITIES_H_

#include <stdbool.h>
#include <stdint.h>

/* Ensure the board pin macros override the driver's fallback pin values. */
#include "../atmel_start_pins.h"
#include "ILI9341/ILI9341_SPI.h"

/*
 * Board-specific ILI9341 callbacks. Pin arguments use the GPIO encoding from
 * atmel_start_pins.h. The SERCOM0 MOSI/SCK pin mux is configured by Atmel
 * Start; CS, DC and RESET are controlled as GPIO outputs.
 */
void ILI9341_PlatformInit(uint8_t mosi_pin, uint8_t sck_pin,
                          uint8_t cs_pin, uint8_t dc_pin, uint8_t reset_pin);
uint8_t ILI9341_PlatformSPITransfer(uint8_t data);
void ILI9341_PlatformPinWrite(uint8_t pin, bool high);
void ILI9341_PlatformDelayMs(uint16_t milliseconds);

#endif /* SW_COMPONENTS_UTILITIES_H_ */
