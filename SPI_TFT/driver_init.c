/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

struct spi_m_sync_descriptor SPI_0;

void SPI_0_PORT_init(void)
{

	gpio_set_pin_level(ILI9341_MOSI_PIN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ILI9341_MOSI_PIN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ILI9341_MOSI_PIN, PINMUX_PA04D_SERCOM0_PAD0);

	gpio_set_pin_level(ILI9341_SCK_PIN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ILI9341_SCK_PIN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ILI9341_SCK_PIN, PINMUX_PA05D_SERCOM0_PAD1);

	// Set pin direction to input
	gpio_set_pin_direction(ILI9341_MISO_PIN, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(ILI9341_MISO_PIN,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(ILI9341_MISO_PIN, PINMUX_PA06D_SERCOM0_PAD2);
}

void SPI_0_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

void SPI_0_init(void)
{
	SPI_0_CLOCK_init();
	spi_m_sync_init(&SPI_0, SERCOM0);
	SPI_0_PORT_init();
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA07

	gpio_set_pin_level(ILI9341_CS_PIN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ILI9341_CS_PIN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ILI9341_CS_PIN, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA08

	gpio_set_pin_level(ILI9341_DC_PIN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ILI9341_DC_PIN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ILI9341_DC_PIN, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA09

	gpio_set_pin_level(ILI9341_RST_PIN,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(ILI9341_RST_PIN, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(ILI9341_RST_PIN, GPIO_PIN_FUNCTION_OFF);

	SPI_0_init();
}
