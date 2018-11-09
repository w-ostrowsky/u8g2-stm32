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

#include "u8g2_io.h"

extern SPI_HandleTypeDef hspi1;

uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg) {
    case U8X8_MSG_BYTE_SEND:
    {
      uint8_t* data = (uint8_t *)arg_ptr;
      HAL_SPI_Transmit(&hspi1, data, arg_int, 100);
    }
     break;

    case U8X8_MSG_BYTE_INIT:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      break;

    case U8X8_MSG_BYTE_SET_DC:
      u8x8_gpio_SetDC(u8x8, arg_int);
      break;

    case U8X8_MSG_BYTE_START_TRANSFER:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
      break;

    case U8X8_MSG_BYTE_END_TRANSFER:
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
      break;

    default:
      return 0;
  }
  return 1;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:  // called once during init phase of u8g2/u8x8
      break;                            // can be used to setup pins
    case U8X8_MSG_DELAY_NANO:           // delay arg_int * 1 nano second
      break;
    case U8X8_MSG_DELAY_100NANO:        // delay arg_int * 100 nano seconds
        __NOP();
      break;
    case U8X8_MSG_DELAY_10MICRO:        // delay arg_int * 10 micro seconds
      HAL_Delay(1);
      break;
    case U8X8_MSG_DELAY_MILLI:          // delay arg_int * 1 milli second
      HAL_Delay(arg_int);
      break;
    case U8X8_MSG_DELAY_I2C:                // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
      break;                            // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
//    case U8X8_MSG_GPIO_D0:              // D0 or SPI clock pin: Output level in arg_int
    case U8X8_MSG_GPIO_SPI_CLOCK:
        if (arg_int) HAL_GPIO_WritePin(GPIOA, CLK_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(GPIOA, CLK_Pin, GPIO_PIN_RESET);
      break;
//    case U8X8_MSG_GPIO_D1:              // D1 or SPI data pin: Output level in arg_int
    case U8X8_MSG_GPIO_SPI_DATA:
        if (arg_int) HAL_GPIO_WritePin(GPIOA, MOSI_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(GPIOA, MOSI_Pin, GPIO_PIN_RESET);
      break;
    case U8X8_MSG_GPIO_D2:              // D2 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D3:              // D3 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D4:              // D4 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D5:              // D5 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D6:              // D6 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_D7:              // D7 pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_E:               // E/WR pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS:              // CS (chip select) pin: Output level in arg_int
        if (arg_int) HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(GPIOA, CS_Pin, GPIO_PIN_RESET);
        break;

    case U8X8_MSG_GPIO_DC:              // DC (data/cmd, A0, register select) pin: Output level in arg_int
        if (arg_int) HAL_GPIO_WritePin(GPIOA, DC_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(GPIOA, DC_Pin, GPIO_PIN_RESET);
        break;

    case U8X8_MSG_GPIO_RESET:           // Reset pin: Output level in arg_int
        if (arg_int) HAL_GPIO_WritePin(GPIOA, RST_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(GPIOA, RST_Pin, GPIO_PIN_RESET);
        break;

    case U8X8_MSG_GPIO_CS1:             // CS1 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS2:             // CS2 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:       // arg_int=0: Output low at I2C clock pin
      break;                            // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:            // arg_int=0: Output low at I2C data pin
      break;                            // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
      break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);          // default return value
      break;
  }
  return 1;
}

