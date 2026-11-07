#include "utilities.h"

#include <atmel_start.h>
#include <hal_delay.h>

/*
 * The SPI pins (MOSI/SCK/MISO) are muxed to SERCOM0 by Atmel Start in
 * driver_init.c. CS, DC and RESET are ordinary GPIO outputs configured
 * here as well as during system_init().
 */
void ILI9341_PlatformInit(uint8_t mosi_pin, uint8_t sck_pin,
                          uint8_t cs_pin, uint8_t dc_pin, uint8_t reset_pin)
{
	/* Keep the SERCOM pin mux configured by Atmel Start. */
	(void)mosi_pin;
	(void)sck_pin;

	/* Set inactive levels before enabling the GPIO outputs. */
	gpio_set_pin_level(cs_pin, true);
	gpio_set_pin_level(dc_pin, true);
	gpio_set_pin_level(reset_pin, true);

	gpio_set_pin_function(cs_pin, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_function(dc_pin, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_function(reset_pin, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_direction(cs_pin, GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(dc_pin, GPIO_DIRECTION_OUT);
	gpio_set_pin_direction(reset_pin, GPIO_DIRECTION_OUT);

	spi_m_sync_enable(&SPI_0);
}

uint8_t ILI9341_PlatformSPITransfer(uint8_t data)
{
	uint8_t received = 0u;
	struct spi_xfer transfer;

	transfer.txbuf = &data;
	transfer.rxbuf = &received;
	transfer.size = 1u;

	/* This is a blocking transfer; the display driver ignores received data. */
	(void)spi_m_sync_transfer(&SPI_0, &transfer);
	return received;
}

void ILI9341_PlatformPinWrite(uint8_t pin, bool high)
{
	gpio_set_pin_level(pin, high);
}

void ILI9341_PlatformDelayMs(uint16_t milliseconds)
{
	delay_ms(milliseconds);
}
