/*
 * u8g2lib.cpp
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

#include "u8g2lib.hpp"
#include "u8g2_io.h"
#include <cstring>

namespace u8g2lib {

template<>
U8G2<CHIP_TYPE::SSD1305, INTERFACE::SPI_4W_SW, DISPLAY::NONAME_128x32, MODE::HALF_PAGE>::U8G2(const u8g2_cb_t *rotation): U8G2()
{
    u8g2_Setup_ssd1305_128x32_noname_1(&u8g2, rotation, u8x8_byte_4wire_sw_spi, u8x8_gpio_and_delay);
}

template<>
U8G2<CHIP_TYPE::SH1106, INTERFACE::SPI_4W_SW, DISPLAY::NONAME_128x64, MODE::HALF_PAGE>::U8G2(const u8g2_cb_t *rotation): U8G2()
{
    u8g2_Setup_sh1106_128x64_noname_1(&u8g2, rotation, u8x8_byte_4wire_sw_spi, u8x8_gpio_and_delay);
}

template<>
U8G2<CHIP_TYPE::SH1106, INTERFACE::SPI_4W_HW, DISPLAY::NONAME_128x64, MODE::HALF_PAGE>::U8G2(const u8g2_cb_t *rotation): U8G2()
{
    u8g2_Setup_sh1106_128x64_noname_1(&u8g2, rotation, u8x8_byte_hw_spi, u8x8_gpio_and_delay);
}

template<>
U8G2<CHIP_TYPE::SH1106, INTERFACE::SPI_4W_SW, DISPLAY::NONAME_128x64, MODE::FULL_PAGE>::U8G2(const u8g2_cb_t *rotation): U8G2()
{
    u8g2_Setup_sh1106_128x64_noname_2(&u8g2, rotation, u8x8_byte_4wire_sw_spi, u8x8_gpio_and_delay);
}

template<>
U8G2<CHIP_TYPE::SH1106, INTERFACE::SPI_4W_HW, DISPLAY::NONAME_128x64, MODE::FULL_PAGE>::U8G2(const u8g2_cb_t *rotation): U8G2()
{
    u8g2_Setup_sh1106_128x64_noname_2(&u8g2, rotation, u8x8_byte_hw_spi, u8x8_gpio_and_delay );
}

template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE MODE>
size_t U8G2<ICT, IO_TYPE, D_NAME, MODE>::write(const char c)
{
    auto enc = cpp_next_cb(&(u8g2.u8x8), c);
    if(enc < 0x0fffe)
    {
        tx += u8g2_DrawGlyph(&u8g2, tx, ty, enc);
        return 1U;
    }
    return 0U;
}

template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE MODE>
size_t U8G2<ICT, IO_TYPE, D_NAME, MODE>::write(const char16_t wc)
{
    auto enc = u8x8_utf8_next(&(u8g2.u8x8), static_cast<uint8_t>(wc >> 8));
    if(enc == 0x0fffe)
    {
        enc = u8x8_utf8_next(&(u8g2.u8x8), static_cast<uint8_t>(wc & 0xFF));
    }
    if(enc < 0x0fffe)
    {
        tx += u8g2_DrawGlyph(&u8g2, tx, ty, enc);
        return 1U;
    }
    return 0U;
}

template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE MODE>
size_t U8G2<ICT, IO_TYPE, D_NAME, MODE>::write(const char *buffer)
{
    return write(buffer, strlen(buffer));
}

template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE MODE>
size_t U8G2<ICT, IO_TYPE, D_NAME, MODE>::write(const char *buffer, uint_fast16_t size)
{
    const auto glyphWidth = u8g2_GetGlyphWidth(&u8g2, cpp_next_cb(&(u8g2.u8x8), '_'));
    const auto availableSpace = (u8g2_GetDisplayWidth(&u8g2) - tx) - (size * glyphWidth);
    if(availableSpace < 0)
    {
        size += (static_cast<decltype(size)>(availableSpace / glyphWidth));
    }
    uint_fast16_t n = 0U;
    for(; size > 0; size--, n++)
    {
        auto enc = cpp_next_cb(&(u8g2.u8x8), buffer[n]);
        if(enc < 0x0fffe)
        {
            tx += u8g2_DrawGlyph(&u8g2, tx, ty, enc);
        }
    }
    return n;
 }

template<CHIP_TYPE ICT, INTERFACE IO_TYPE, DISPLAY D_NAME, MODE MODE>
void U8G2<ICT, IO_TYPE, D_NAME, MODE>::writeln()
{
    const decltype(ty) nLine = ty + static_cast<decltype(ty)>(u8g2_GetMaxCharHeight(&u8g2));
    if(static_cast<std::make_signed_t<decltype(nLine)>>(u8g2_GetDisplayHeight(&u8g2) - nLine) > -1)
    {
        ty = nLine;
    }
    tx = 0U;
}

}
