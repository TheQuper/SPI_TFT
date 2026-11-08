/*
 * XPT2046 resistive touchscreen driver for Atmel Start / SAM D21.
 *
 * Protocol and filtering approach adapted from:
 * https://github.com/PaulStoffregen/XPT2046_Touchscreen
 * Copyright (c) 2015, Paul Stoffregen, paul@pjrc.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
	 * The above copyright notice, development funding notice, and permission
	 * notice shall be included in all copies or substantial portions of the
	 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "XPT2046_Touchscreen.h"

#include <hal_gpio.h>

#define XPT2046_Z1_COMMAND 0xB1u
#define XPT2046_Z2_COMMAND 0x00C1u
#define XPT2046_X_COMMAND  0x0091u
#define XPT2046_Y_COMMAND  0x00D1u
#define XPT2046_Y_POWERDOWN_COMMAND 0x00D0u

static bool transfer_bytes(XPT2046_Touchscreen *touch, const uint8_t *tx,
		uint8_t *rx, uint16_t size)
{
	struct spi_xfer transfer;
	transfer.txbuf = tx;
	transfer.rxbuf = rx;
	transfer.size = size;
	return spi_m_sync_transfer(touch->spi, &transfer) == (int32_t)size;
}

static bool transfer_u8(XPT2046_Touchscreen *touch, uint8_t tx)
{
	uint8_t rx = 0u;
	return transfer_bytes(touch, &tx, &rx, 1u);
}

static bool transfer_u16(XPT2046_Touchscreen *touch, uint16_t tx, uint16_t *rx)
{
	uint8_t tx_bytes[2];
	uint8_t rx_bytes[2] = {0u, 0u};
	tx_bytes[0] = (uint8_t)(tx >> 8);
	tx_bytes[1] = (uint8_t)tx;
	if (!transfer_bytes(touch, tx_bytes, rx_bytes, 2u))
		return false;
	*rx = (uint16_t)(((uint16_t)rx_bytes[0] << 8) | rx_bytes[1]);
	return true;
}

static int16_t best_two_average(int16_t a, int16_t b, int16_t c)
{
	int16_t ab = (a > b) ? (int16_t)(a - b) : (int16_t)(b - a);
	int16_t ac = (a > c) ? (int16_t)(a - c) : (int16_t)(c - a);
	int16_t bc = (b > c) ? (int16_t)(b - c) : (int16_t)(c - b);
	if (ab <= ac && ab <= bc)
		return (int16_t)((a + b) >> 1);
	if (ac <= ab && ac <= bc)
		return (int16_t)((a + c) >> 1);
	return (int16_t)((b + c) >> 1);
}

bool XPT2046_Touchscreen_Init(XPT2046_Touchscreen *touch,
		struct spi_m_sync_descriptor *spi, uint8_t cs_pin, uint8_t irq_pin)
{
	if (touch == 0 || spi == 0)
		return false;

	touch->spi = spi;
	touch->cs_pin = cs_pin;
	touch->irq_pin = irq_pin;
	touch->rotation = 1u;
	touch->z_threshold = XPT2046_DEFAULT_Z_THRESHOLD;

	/* The SPI peripheral itself is configured by Atmel Start. */
	spi_m_sync_enable(spi);
	gpio_set_pin_level(cs_pin, true);
	gpio_set_pin_function(cs_pin, GPIO_PIN_FUNCTION_OFF);
	gpio_set_pin_direction(cs_pin, GPIO_DIRECTION_OUT);

	if (irq_pin != XPT2046_IRQ_NONE) {
		gpio_set_pin_function(irq_pin, GPIO_PIN_FUNCTION_OFF);
		gpio_set_pin_direction(irq_pin, GPIO_DIRECTION_IN);
		gpio_set_pin_pull_mode(irq_pin, GPIO_PULL_UP);
	}
	return true;
}

void XPT2046_Touchscreen_SetRotation(XPT2046_Touchscreen *touch, uint8_t rotation)
{
	if (touch != 0)
		touch->rotation = (uint8_t)(rotation & 3u);
}

void XPT2046_Touchscreen_SetZThreshold(XPT2046_Touchscreen *touch, uint16_t threshold)
{
	if (touch != 0)
		touch->z_threshold = threshold;
}

bool XPT2046_Touchscreen_ReadPoint(XPT2046_Touchscreen *touch, XPT2046_Point *point)
{
	uint16_t samples[6];
	uint16_t value;
	int32_t pressure;
	int16_t raw_x;
	int16_t raw_y;
	uint8_t i;
	bool ok = true;

	if (touch == 0 || point == 0 || touch->spi == 0)
		return false;
	point->x = 0;
	point->y = 0;
	point->z = 0;

	/* T_IRQ is active low; omit the bus transaction when untouched. */
	if (touch->irq_pin != XPT2046_IRQ_NONE && gpio_get_pin_level(touch->irq_pin))
		return false;

	gpio_set_pin_level(touch->cs_pin, false);
	ok = transfer_u8(touch, XPT2046_Z1_COMMAND);
	if (ok && transfer_u16(touch, XPT2046_Z2_COMMAND, &value))
		pressure = (int32_t)(value >> 3) + 4095;
	else
		ok = false;
	if (ok && transfer_u16(touch, XPT2046_X_COMMAND, &value))
		pressure -= value >> 3;
	else
		ok = false;

	if (ok && pressure >= touch->z_threshold) {
		/* Discard the first X conversion, then take three X/Y samples. */
		ok = transfer_u16(touch, XPT2046_X_COMMAND, &value);
		for (i = 0u; ok && i < 4u; ++i) {
			uint16_t command = (i & 1u) ? XPT2046_X_COMMAND : XPT2046_Y_COMMAND;
			ok = transfer_u16(touch, command, &value);
			if (ok)
				samples[i] = (uint16_t)(value >> 3);
		}
	} else {
		samples[0] = samples[1] = samples[2] = samples[3] = 0u;
	}

	/* Power down the ADC and finish the transaction, even after a read error. */
	if (ok && transfer_u16(touch, XPT2046_Y_POWERDOWN_COMMAND, &value))
		samples[4] = (uint16_t)(value >> 3);
	else if (ok)
		ok = false;
	if (ok && transfer_u16(touch, 0u, &value))
		samples[5] = (uint16_t)(value >> 3);
	else if (ok)
		ok = false;
	gpio_set_pin_level(touch->cs_pin, true);
	if (!ok)
		return false;

	if (pressure < 0)
		pressure = 0;
	if (pressure < touch->z_threshold)
		return false;

	raw_x = best_two_average((int16_t)samples[1], (int16_t)samples[3],
			(int16_t)samples[5]);
	raw_y = best_two_average((int16_t)samples[0], (int16_t)samples[2],
			(int16_t)samples[4]);

	switch (touch->rotation) {
	case 0u:
		point->x = (int16_t)(4095 - raw_y);
		point->y = raw_x;
		break;
	case 1u:
		point->x = raw_x;
		point->y = raw_y;
		break;
	case 2u:
		point->x = raw_y;
		point->y = (int16_t)(4095 - raw_x);
		break;
	default:
		point->x = (int16_t)(4095 - raw_x);
		point->y = (int16_t)(4095 - raw_y);
		break;
	}
	point->z = (int16_t)((pressure > 32767) ? 32767 : pressure);
	return true;
}
