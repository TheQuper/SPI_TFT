#include <atmel_start.h>
#include <hal_delay.h>
#include "SW_Components/ILI9341/GFX_Library_SPI/GFX_Library_SPI.h"
#include "SW_Components/ILI9341/ILI9341_SPI.h"

/*
Odczyt danych z wyœwietlacza ILI po miso

Po ILI9341_Init() mo¿esz wywo³aæ:

- ILI9341_ReadDisplayId(id) — 3 bajty identyfikatora;
- ILI9341_ReadDisplayStatus(status) — 4 bajty statusu;
- ILI9341_ReadPowerMode(&mode);
- ILI9341_ReadMadctl(&value);
- ILI9341_ReadPixelFormat(&value);
- albo ogólne ILI9341_ReadRegister(command, data, length).

Odczyt uwzglêdnia pusty bajt przed danymi; dla komendy ID kontroler zwraca najpierw ten bajt, a potem trzy bajty identyfikatora. Karta katalogowa ILI9341 (https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)

Funkcje s¹ w ILI9341_SPI.c (SPI_TFT/SW_Components/ILI9341/ILI9341_SPI.c) i zadeklarowane w ILI9341_SPI.h (SPI_TFT/SW_Components/ILI9341/ILI9341_SPI.h).
*/

int main(void)
{
	unsigned int counter = 0u;

	atmel_start_init();
	ILI9341_Init();
	delay_ms(500u);
	fillScreen(0x0000);

	drawPixel(0, 0, 0xFFFF);     /* White pixel at top-left */
	drawPixel(239, 319, 0xF800); /* Red pixel at bottom-right */

	display_setTextSize(1);
	display_setTextColor(0xFFFF, 0x0000);
	display_setCursor(10, 10);
	display_printf("Hello, world!\r\n");
	display_printf("%4.2f\r\n", 3.141592);
delay_ms(500u);
	while (1) {
		display_setCursor(10, 34);
		display_printf("Counter: %04lu", (unsigned long)counter);
		counter = (counter + 1u) % 10000u;
		delay_ms(1000u);
	}
}
