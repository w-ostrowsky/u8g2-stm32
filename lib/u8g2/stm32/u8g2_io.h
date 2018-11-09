/*
 * u8g2_io.h
 *
 *  Created on: 21 Oct 2017
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

#ifndef U8G2_IO_HPP
#define U8G2_IO_HPP

#ifdef __cplusplus
 extern "C" {

#endif

#include "../u8g2/csrc/u8g2.h"
#include "stm32l4xx_hal.h"

#define RST_Pin GPIO_PIN_0
#define CLK_Pin GPIO_PIN_1
#define MOSI_Pin GPIO_PIN_7
#define DC_Pin GPIO_PIN_3
#define CS_Pin GPIO_PIN_4

uint8_t u8x8_byte_hw_spi(u8x8_t*, uint8_t, uint8_t, void*);
uint8_t u8x8_gpio_and_delay(u8x8_t*, uint8_t, uint8_t, void*);


#ifdef __cplusplus
}
#endif

#endif
