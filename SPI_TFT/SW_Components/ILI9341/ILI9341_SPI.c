#include "ILI9341_SPI.h"
#include "GFX_Library_SPI/GFX_Library_SPI.h"
#include "atmel_start_pins.h"

#define CMD_SWRESET 0x01u
#define CMD_SLPOUT  0x11u
#define CMD_DISPON  0x29u
#define CMD_CASET   0x2Au
#define CMD_PASET   0x2Bu
#define CMD_RAMWR   0x2Cu
#define CMD_MADCTL  0x36u
#define CMD_PIXFMT  0x3Au
#define CMD_RDDID   0x04u
#define CMD_RDDST   0x09u
#define CMD_RDDPM   0x0Au
#define CMD_RDDMADCTL 0x0Bu
#define CMD_RDDCOLMOD 0x0Cu
#define CMD_INVON   0x21u
#define CMD_INVOFF  0x20u

#define MADCTL_MY   0x80u
#define MADCTL_MX   0x40u
#define MADCTL_MV   0x20u
#define MADCTL_BGR  0x08u

uint16_t _width = ILI9341_WIDTH;
uint16_t _height = ILI9341_HEIGHT;
uint8_t rotation = 0u;

static void writeCommand(uint8_t command)
{
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, false);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, false);
    (void)ILI9341_PlatformSPITransfer(command);
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
}

static void writeData(uint8_t data)
{
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, false);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, true);
    (void)ILI9341_PlatformSPITransfer(data);
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
}

static void writeCommandData(uint8_t command, const uint8_t *data, uint8_t count)
{
    uint8_t i;
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, false);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, false);
    (void)ILI9341_PlatformSPITransfer(command);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, true);
    for (i = 0u; i < count; ++i)
        (void)ILI9341_PlatformSPITransfer(data[i]);
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
}

static void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];
    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)x0;
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)x1;
    writeCommandData(CMD_CASET, data, 4u);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)y0;
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)y1;
    writeCommandData(CMD_PASET, data, 4u);
    writeCommand(CMD_RAMWR);
}

static void writeColor(uint16_t color, uint32_t count)
{
    uint8_t high = (uint8_t)(color >> 8);
    uint8_t low = (uint8_t)color;
    uint32_t i;
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, false);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, true);
    for (i = 0u; i < count; ++i) {
        (void)ILI9341_PlatformSPITransfer(high);
        (void)ILI9341_PlatformSPITransfer(low);
    }
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
}

void ILI9341_Init(void)
{
    ILI9341_PlatformInit(ILI9341_MOSI_PIN, ILI9341_SCK_PIN,
                         ILI9341_CS_PIN, ILI9341_DC_PIN, ILI9341_RST_PIN);
    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, true);
#if ILI9341_USE_RESET
    ILI9341_PlatformPinWrite(ILI9341_RST_PIN, false);
    ILI9341_PlatformDelayMs(10u);
    ILI9341_PlatformPinWrite(ILI9341_RST_PIN, true);
    ILI9341_PlatformDelayMs(120u);
#endif

    writeCommand(CMD_SWRESET);
    ILI9341_PlatformDelayMs(150u);

    /* Power, VCOM, frame rate and display-function setup from the Adafruit
       reference initialization sequence for common ILI9341 modules. */
    { const uint8_t d[] = {0x03u, 0x80u, 0x02u}; writeCommandData(0xEFu, d, 3u); }
    { const uint8_t d[] = {0x00u, 0xC1u, 0x30u}; writeCommandData(0xCFu, d, 3u); }
    { const uint8_t d[] = {0x64u, 0x03u, 0x12u, 0x81u}; writeCommandData(0xEDu, d, 4u); }
    { const uint8_t d[] = {0x85u, 0x00u, 0x78u}; writeCommandData(0xE8u, d, 3u); }
    { const uint8_t d[] = {0x39u, 0x2Cu, 0x00u, 0x34u, 0x02u}; writeCommandData(0xCBu, d, 5u); }
    { const uint8_t d[] = {0x20u}; writeCommandData(0xF7u, d, 1u); }
    { const uint8_t d[] = {0x00u, 0x00u}; writeCommandData(0xEAu, d, 2u); }
    { const uint8_t d[] = {0x23u}; writeCommandData(0xC0u, d, 1u); }
    { const uint8_t d[] = {0x10u}; writeCommandData(0xC1u, d, 1u); }
    { const uint8_t d[] = {0x3Eu, 0x28u}; writeCommandData(0xC5u, d, 2u); }
    { const uint8_t d[] = {0x86u}; writeCommandData(0xC7u, d, 1u); }
    { const uint8_t d[] = {0x55u}; writeCommandData(CMD_PIXFMT, d, 1u); }
    { const uint8_t d[] = {0x00u, 0x18u}; writeCommandData(0xB1u, d, 2u); }
    { const uint8_t d[] = {0x08u, 0x82u, 0x27u}; writeCommandData(0xB6u, d, 3u); }
    { const uint8_t d[] = {0x00u}; writeCommandData(0xF2u, d, 1u); }
    { const uint8_t d[] = {0x01u}; writeCommandData(0x26u, d, 1u); }
    { const uint8_t d[] = {0x0Fu,0x31u,0x2Bu,0x0Cu,0x0Eu,0x08u,0x4Eu,0xF1u,0x37u,0x07u,0x10u,0x03u,0x0Eu,0x09u,0x00u}; writeCommandData(0xE0u, d, 15u); }
    { const uint8_t d[] = {0x00u,0x0Eu,0x14u,0x03u,0x11u,0x07u,0x31u,0xC1u,0x48u,0x08u,0x0Fu,0x0Cu,0x31u,0x36u,0x0Fu}; writeCommandData(0xE1u, d, 15u); }

    writeCommand(CMD_SLPOUT);
    ILI9341_PlatformDelayMs(120u);
    ILI9341_SetRotation(0u);
    writeCommand(CMD_DISPON);
    ILI9341_PlatformDelayMs(20u);

}

void ILI9341_SetRotation(uint8_t value)
{
    uint8_t madctl;
    rotation = (uint8_t)(value & 3u);
    switch (rotation) {
    case 0u: madctl = MADCTL_MX | MADCTL_BGR; _width = 240u; _height = 320u; break;
    case 1u: madctl = MADCTL_MV | MADCTL_BGR; _width = 320u; _height = 240u; break;
    case 2u: madctl = MADCTL_MY | MADCTL_BGR; _width = 240u; _height = 320u; break;
    default: madctl = MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR; _width = 320u; _height = 240u; break;
    }
    writeCommandData(CMD_MADCTL, &madctl, 1u);
}

void ILI9341_InvertDisplay(bool invert)
{
    writeCommand(invert ? CMD_INVON : CMD_INVOFF);
}

bool ILI9341_ReadRegister(uint8_t command, uint8_t *data, uint8_t length)
{
    uint8_t i;
    if (data == 0 || length == 0u)
        return false;

    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, false);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, false);
    (void)ILI9341_PlatformSPITransfer(command);
    ILI9341_PlatformPinWrite(ILI9341_DC_PIN, true);

    /* Read commands return a dummy byte before their first parameter. */
    (void)ILI9341_PlatformSPITransfer(0x00u);
    for (i = 0u; i < length; ++i)
        data[i] = ILI9341_PlatformSPITransfer(0x00u);

    ILI9341_PlatformPinWrite(ILI9341_CS_PIN, true);
    return true;
}

bool ILI9341_ReadDisplayId(uint8_t id[3])
{
    return ILI9341_ReadRegister(CMD_RDDID, id, 3u);
}

bool ILI9341_ReadDisplayStatus(uint8_t status[4])
{
    return ILI9341_ReadRegister(CMD_RDDST, status, 4u);
}

bool ILI9341_ReadPowerMode(uint8_t *mode)
{
    return ILI9341_ReadRegister(CMD_RDDPM, mode, 1u);
}

bool ILI9341_ReadMadctl(uint8_t *madctl)
{
    return ILI9341_ReadRegister(CMD_RDDMADCTL, madctl, 1u);
}

bool ILI9341_ReadPixelFormat(uint8_t *format)
{
    return ILI9341_ReadRegister(CMD_RDDCOLMOD, format, 1u);
}

uint16_t ILI9341_GetWidth(void) { return _width; }
uint16_t ILI9341_GetHeight(void) { return _height; }

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= _width || y >= _height) return;
    setAddressWindow(x, y, x, y);
    writeColor(color, 1u);
}

void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t count;
    if (w == 0u || h == 0u || x >= _width || y >= _height) return;
    if ((uint32_t)x + w > _width) w = (uint16_t)(_width - x);
    if ((uint32_t)y + h > _height) h = (uint16_t)(_height - y);
    setAddressWindow(x, y, (uint16_t)(x + w - 1u), (uint16_t)(y + h - 1u));
    count = (uint32_t)w * h;
    writeColor(color, count);
}

void ILI9341_DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{ ILI9341_FillRect(x, y, w, 1u, color); }

void ILI9341_DrawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{ ILI9341_FillRect(x, y, 1u, h, color); }

void ILI9341_FillScreen(uint16_t color)
{ ILI9341_FillRect(0u, 0u, _width, _height, color); }

/* Names expected by GFX_Library_SPI. */
void drawPixel(uint16_t x, uint16_t y, uint16_t color) { ILI9341_DrawPixel(x, y, color); }
void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) { ILI9341_DrawHLine(x, y, w, color); }
void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) { ILI9341_DrawVLine(x, y, h, color); }
void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) { ILI9341_FillRect(x, y, w, h, color); }
void fillScreen(uint16_t color) { ILI9341_FillScreen(color); }
void setRotation(uint8_t r) { ILI9341_SetRotation(r); }
void invertDisplay(bool invert) { ILI9341_InvertDisplay(invert); }
