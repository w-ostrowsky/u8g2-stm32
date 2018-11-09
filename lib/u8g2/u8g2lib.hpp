/*
 * u8g2lib.hpp
 *
 *  Created on: 15 Oct 2017
 *      Author: Władysław Ostrowski
 */

/*
 This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef U8G2_U8G2LIB_HPP
#define U8G2_U8G2LIB_HPP

#include "u8g2/csrc/u8g2.h"
#include "Print.hpp"

namespace u8g2lib {

using Coord_t =
#ifdef U8G2_16BIT
        uint_fast16_t
#else
        uint_fast8_t
#endif
;

enum class CHIP_TYPE
{
    HX1230, IL3820, IST3020, KS0108, LC7981, LD7032, LS013B7DH03, LS027B7DH01, MAX7219,
    NT7534, PCD8544, PCF8812, RA8835,
    SBN1661, SED1520, SH1106, SH1107, SH1108, SH1122, SSD0323, SSD1305, SSD1306,
    SSD1309, SSD1317, SSD1322, SSD1326, SSD1327, SSD1325, SSD1329,
    SSD1606, SSD1607,
    ST7565, ST7567, ST7586S, ST7588, ST7920, ST75256, T6963,
    UC1601, UC1604, UC1608, UC1610, UC1611, UC1638, UC1701,
};

enum class INTERFACE
{
    SPI_4W_SW, // 4-wire (clock, data, cs and dc) software emulated SPI
    SPI_4W_HW, // 4-wire (clock, data, cs and dc) hardware SPI
    SPI_2ND_4W_HW, // If supported, second 4-wire hardware SPI
    SPI_3W_SW, // 3-wire (clock, data and cs) software emulated SPI
    SPI_3W_HW, // 3-wire (clock, data and cs) hardware SPI
    SPI_HW_IT, // Hardware SPI, data flow based on interrupts
    SPI_HW_DMA, // Hardware SPI, data flow based on DMA
    I2C_SW, // Software emulated I2C/TWI
    I2C_HW, // Hardware I2C
    I2C_2ND_HW, // If supported, use second hardware I2C
    I8080,  // A 8-Bit bus which requires 8 data lines, chip select and a write strobe signal.
    M6800   // 8-bit parallel interface, 6800 protocol
};
enum class DISPLAY
{
    NONE, NONAME_60x32, NONAME_64x32, NONAME_64x48, NONAME_96x16, NONAME_96x96,
    NONAME_128x32,
    ADAFRUIT_128x32, NONAME_128x64, NONAME_128x96, NONAME_128x128
    , NONAME_192x32
};

enum class MODE
{
    HALF_PAGE,   // Use a firstPage()/nextPage() loop for drawing on the display.
    FULL_PAGE,   // same as HALF_PAGE but keeps full frame in RAM
    FULL_BUFFER, //Keep a copy of the full display frame buffer in the RAM.
                 //Use clearBuffer() to clear the RAM
                 //and sendBuffer() to transfer the RAM to the display.
    U8x8         // character only mode
};

using Print::Print;

void u8g2_SetPageCurrTileRow(const u8g2_t *, const uint8_t);


template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE M>
class U8G2: public Print
{
public:
    U8G2(const u8g2_cb_t *rotation);
    U8G2(const U8G2&) = delete;
    U8G2(const U8G2&&) = delete;
    U8G2& operator=(const U8G2&) = delete;
    U8G2& operator=(const U8G2&&) = delete;

    void enableUTF8Print() { cpp_next_cb = u8x8_utf8_next; }
    void disableUTF8Print() { cpp_next_cb = u8x8_ascii_next; }

    auto* getU8x8() { return u8g2_GetU8x8(&u8g2); }
    auto* getU8g2() { return &u8g2; }

    void begin()
    {
        u8g2_InitDisplay(&u8g2);
        u8g2_ClearDisplay(&u8g2);
        u8g2_SetPowerSave(&u8g2, 0U);
    }

    void initDisplay() { u8g2_InitDisplay(&u8g2); }
    void clear() { home(); clearDisplay(); clearBuffer(); }
    void clearDisplay() { u8g2_ClearDisplay(&u8g2); }
    void setPowerSave(const uint8_t is_enable) { u8g2_SetPowerSave(&u8g2, is_enable); }
    void setFlipMode(const uint8_t mode) { u8g2_SetFlipMode(&u8g2, mode); }
    void noDisplay() { u8g2_SetPowerSave(&u8g2, 1U); }
    void display() { u8g2_SetPowerSave(&u8g2, 0U); }
    void setContrast(const uint8_t value)
    {
        u8g2_SetContrast(&u8g2, value);
    }
    void setDisplayRotation(const u8g2_cb_t *u8g2_cb) {u8g2_SetDisplayRotation(&u8g2, u8g2_cb); }
    void home()
    {
        setCursor();
        u8x8_utf8_init(u8g2_GetU8x8(&u8g2));
    }

    /* u8g2_buffer.c */
    void sendBuffer() { u8g2_SendBuffer(&u8g2); }
    void clearBuffer() { u8g2_ClearBuffer(&u8g2); }

    void firstPage() { u8g2_FirstPage(&u8g2); }
    bool nextPage()
     {
         auto ret = 0U != u8g2_NextPage(&u8g2);
         if(ret)
         {
             tx = rTx;
             ty = rTy;
         }
         else
         {
             rTx = tx;
             rTy = ty;
         }
         return ret;
     }

    uint8_t *getBufferPtr() { return u8g2_GetBufferPtr(&u8g2); }
    uint_fast8_t getBufferTileHeight() { return u8g2_GetBufferTileHeight(&u8g2); }
    uint_fast8_t getBufferTileWidth() { return u8g2_GetBufferTileWidth(&u8g2); }
    uint_fast8_t getPageCurrTileRow() { return u8g2_GetPageCurrTileRow(&u8g2); }
    void setPageCurrTileRow(const uint8_t row) { u8g2_SetPageCurrTileRow(&u8g2, row); }
    void setAutoPageClear(uint_fast8_t mode)  { u8g2_SetAutoPageClear(&u8g2, mode); }

    void setCursor(const uint_fast8_t x = 0U, const uint_fast8_t y = 0U)
    {
        if(static_cast<int_fast8_t>((u8g2_GetDisplayWidth(&u8g2) - y)) > -1)
        {
            ty = rTy = y;
        }
        tx = rTx = x;
    }

    Coord_t drawStr(const uint8_t x, const uint8_t y, const char* const s)
    {
        return u8g2_DrawStr(&u8g2, x, y, s);
    }

    Coord_t drawGlyph(const uint8_t x, const uint8_t y, const uint16_t encoding)
    {
        return u8g2_DrawGlyph(&u8g2, x, y, encoding);
    }

    Coord_t drawUTF8(const uint8_t x, const uint8_t y, const char *s)
    {
        return u8g2_DrawUTF8(&u8g2, x, y, s);
    }

    uint_fast8_t drawExtUTF8(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t to_left, const uint16_t *kerning_table, const char *s)
    {
        return u8g2_DrawExtUTF8(&u8g2, x, y, to_left, kerning_table, s);
    }

    Coord_t getStrWidth(const char *s)
    {
        auto strPixelLen = 0U;
        if(cpp_next_cb == u8x8_utf8_next) // should be common way to get length for utf and not utf strings
        {
            strPixelLen = u8g2_GetUTF8Width(&u8g2, s);
        }
        else
        {
            strPixelLen = u8g2_GetStrWidth(&u8g2, s);
        }
        return strPixelLen;
    }

    void sleepOn() { u8g2_SetPowerSave(&u8g2, 1U); }
    void sleepOff() { u8g2_SetPowerSave(&u8g2, 0U); }
    void setColorIndex(uint8_t color_index) { u8g2_SetDrawColor(&u8g2, color_index); }
    uint_fast8_t getColorIndex() { return u8g2_GetDrawColor(&u8g2); }
    int_fast8_t getFontAscent() { return u8g2_GetAscent(&u8g2); }
    int_fast8_t getFontDescent() { return u8g2_GetDescent(&u8g2); }
    Coord_t getHeight() { return u8g2_GetDisplayHeight(&u8g2); }
    Coord_t getWidth() { return u8g2_GetDisplayWidth(&u8g2); }

    /* u8g2.hvline.c */
    void setDrawColor(uint8_t color_index) { u8g2_SetDrawColor(&u8g2, color_index); }
    uint_fast8_t getDrawColor() { return u8g2_GetDrawColor(&u8g2); }
    void drawPixel(const uint_fast8_t x, const uint_fast8_t y) { u8g2_DrawPixel(&u8g2, x, y); }
    void drawHLine(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t w) { u8g2_DrawHLine(&u8g2, x, y, w); }
    void drawVLine(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t h) { u8g2_DrawVLine(&u8g2, x, y, h); }
    void drawHVLine(const uint_fast8_t x, const uint_fast8_t y, const uint_fast8_t len, const uint_fast8_t dir) {
      u8g2_DrawHVLine(&u8g2, x, y, len, dir); }

    /* u8g2_box.c */
    void drawFrame(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h) { u8g2_DrawFrame(&u8g2, x, y, w, h); }
    void drawRFrame(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint8_t r) { u8g2_DrawRFrame(&u8g2, x, y, w, h,r); }
    void drawBox(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h) { u8g2_DrawBox(&u8g2, x, y, w, h); }
    void drawRBox(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint8_t r) { u8g2_DrawRBox(&u8g2, x, y, w, h,r); }

    /* u8g2_circle.c */
    void drawCircle(const uint8_t x0, const uint8_t y0, const uint8_t rad, const uint8_t opt = U8G2_DRAW_ALL) { u8g2_DrawCircle(&u8g2, x0, y0, rad, opt); }
    void drawDisc(const uint8_t x0, const uint8_t y0, const uint8_t rad, const uint8_t opt = U8G2_DRAW_ALL) { u8g2_DrawDisc(&u8g2, x0, y0, rad, opt); }
    void drawEllipse(const uint8_t x0, const uint8_t y0, const uint8_t rx, const uint8_t ry, const uint8_t opt = U8G2_DRAW_ALL) { u8g2_DrawEllipse(&u8g2, x0, y0, rx, ry, opt); }
    void drawFilledEllipse(const uint8_t x0, const uint8_t y0, const uint8_t rx, const uint8_t ry, const uint8_t opt = U8G2_DRAW_ALL) { u8g2_DrawFilledEllipse(&u8g2, x0, y0, rx, ry, opt); }

    /* u8g2_line.c */
    void drawLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t y2)
      { u8g2_DrawLine(&u8g2, x1, y1, x2, y2); }

    /* u8g2_bitmap.c */
    void setBitmapMode(const uint8_t is_transparent)
      { u8g2_SetBitmapMode(&u8g2, is_transparent); }
    void drawBitmap(const uint8_t x, const uint8_t y, const uint8_t cnt, const uint8_t h, const uint8_t *bitmap)
      { u8g2_DrawBitmap(&u8g2, x, y, cnt, h, bitmap); }
    void drawXBM(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint8_t *bitmap)
      { u8g2_DrawXBM(&u8g2, x, y, w, h, bitmap); }
    void drawXBMP(const uint8_t x, const uint8_t y, const uint8_t w, const uint8_t h, const uint8_t *bitmap)
      { u8g2_DrawXBMP(&u8g2, x, y, w, h, bitmap); }

    /* u8g2_polygon.c */
    void drawTriangle(const int16_t x0, const int16_t y0, const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2)
      { u8g2_DrawTriangle(&u8g2, x0, y0, x1, y1, x2, y2); }

    /* u8g2_font.c */

    void setFont(const uint8_t* font)
     {
         u8g2_SetFont(&u8g2, font);
     }
    void setFontMode(uint8_t  is_transparent) {u8g2_SetFontMode(&u8g2, is_transparent); }
    void setFontDirection(uint8_t dir) {u8g2_SetFontDirection(&u8g2, dir); }

    uint_fast8_t getAscent() { return u8g2_GetAscent(&u8g2); }
    uint_fast8_t getDescent() { return u8g2_GetDescent(&u8g2); }

    void setFontPosBaseline() { u8g2_SetFontPosBaseline(&u8g2); }
    void setFontPosBottom() { u8g2_SetFontPosBottom(&u8g2); }
    void setFontPosTop() { u8g2_SetFontPosTop(&u8g2); }
    void setFontPosCenter() { u8g2_SetFontPosCenter(&u8g2); }

    void setFontRefHeightText() { u8g2_SetFontRefHeightText(&u8g2); }
    void setFontRefHeightExtendedText() { u8g2_SetFontRefHeightExtendedText(&u8g2); }
    void setFontRefHeightAll() { u8g2_SetFontRefHeightAll(&u8g2); }

    // user interface
    uint_fast8_t userInterfaceSelectionList(const char *title, uint8_t start_pos, const char *sl) {
      return u8g2_UserInterfaceSelectionList(&u8g2, title, start_pos, sl); }
    uint_fast8_t userInterfaceMessage(const char *title1, const char *title2, const char *title3, const char *buttons) {
      return u8g2_UserInterfaceMessage(&u8g2, title1, title2, title3, buttons); }
    uint_fast8_t userInterfaceInputValue(const char *title, const char *pre, uint8_t *value, uint8_t lo, uint8_t hi, uint8_t digits, const char *post) {
      return u8g2_UserInterfaceInputValue(&u8g2, title, pre, value, lo, hi, digits, post); }

protected:
    u8x8_char_cb cpp_next_cb = u8x8_ascii_next;
    virtual size_t write(const char) override;
    virtual size_t write(const char16_t) override;
    virtual void writeln() override;
    virtual size_t write(const char*) override;
    virtual size_t write(const char*, uint_fast16_t) override;

private:
    U8G2() = default;
    u8g2_t u8g2;
    Coord_t rTx = 0U, rTy = 0U;
    Coord_t tx = 0U, ty = 0U;
};

}
#endif /* U8G2_U8G2LIB_HPP_ */
