#include <atmel_start.h>
#include <hal_delay.h>
#include "SW_Components/ILI9341/GFX_Library_SPI/GFX_Library_SPI.h"
#include "SW_Components/ILI9341/ILI9341_SPI.h"

int main(void)
{
	unsigned int counter = 0u;

	atmel_start_init();
	ILI9341_Init();
	fillScreen(0x0001);

	drawPixel(0, 0, 0xFFFF);     /* White pixel at top-left */
	drawPixel(239, 319, 0xF800); /* Red pixel at bottom-right */

	display_setTextSize(1);
	display_setTextColor(0xFFFF, 0x0000);
	display_setCursor(10, 10);
	display_printf("Hello, world!\r\n");
	display_printf("%4.2f\r\n", 3.141592);

	while (1) {
		display_setCursor(10, 34);
		display_printf("Counter: %04lu", (unsigned long)counter);
		counter = (counter + 1u) % 10000u;
		delay_ms(500u);
	}
}
