/*
 * XPT2046 resistive touchscreen driver for Atmel Start / SAM D21.
 *
 * Protocol and filtering approach adapted from Paul Stoffregen's
 * XPT2046_Touchscreen library. See the license notice in the .c file.
 */
#ifndef XPT2046_TOUCHSCREEN_H_
#define XPT2046_TOUCHSCREEN_H_

#include <stdbool.h>
#include <stdint.h>
#include <hal_spi_m_sync.h>

#define XPT2046_IRQ_NONE       0xFFu
#define XPT2046_DEFAULT_Z_THRESHOLD 300u

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} XPT2046_Point;

typedef struct {
	struct spi_m_sync_descriptor *spi;
	uint8_t cs_pin;
	uint8_t irq_pin;
	uint8_t rotation;
	uint16_t z_threshold;
} XPT2046_Touchscreen;

/*
 * spi must already be initialized by Atmel Start. irq_pin may be
 * XPT2046_IRQ_NONE when the module's T_IRQ signal is not connected.
 */
bool XPT2046_Touchscreen_Init(XPT2046_Touchscreen *touch,
		struct spi_m_sync_descriptor *spi, uint8_t cs_pin, uint8_t irq_pin);

/* Returns false when no touch is detected or an SPI transfer fails. */
bool XPT2046_Touchscreen_ReadPoint(XPT2046_Touchscreen *touch, XPT2046_Point *point);
void XPT2046_Touchscreen_SetRotation(XPT2046_Touchscreen *touch, uint8_t rotation);
void XPT2046_Touchscreen_SetZThreshold(XPT2046_Touchscreen *touch, uint16_t threshold);

#endif /* XPT2046_TOUCHSCREEN_H_ */
