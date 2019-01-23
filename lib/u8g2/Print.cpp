/*
 * Print.cpp
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

#include "Print.hpp"
#include <algorithm>
#include <iterator>
#include <cmath>

using namespace std;

namespace Print
{

  uint_fast8_t
  Print::print (char c)
  {
    return write (c);
  }

  uint_fast8_t
  Print::println (char c)
  {
    write (c);
    writeln ();
    return 1U;
  }

  uint_fast8_t
  Print::print (const char* const s)
  {
    return write (s);
  }

  uint_fast8_t
  Print::println (const char* const s)
  {
    const auto n = write (s);
    writeln ();
    return n;
  }
  uint_fast8_t
  Print::print (const string& s)
  {
    return write (s.c_str ());
  }

  uint_fast8_t
  Print::println (const string& s)
  {
    const auto n = write (s.c_str ());
    writeln ();
    return n;
  }

  uint_fast8_t
  Print::print (double number, uint_fast8_t digits, const bool fixDP)
  {
    if (checkFloat (number) == NUM_ERRORS::Ok)
      {
        if (m_isNegative)
          number = -number;
        if (checkFloat (number, digits) == NUM_ERRORS::Ok)
          {
            printFloatNumber (fixDP);
          }
        else
          {
            markOutOfRange ();
          }
      }
    show ();
    return m_intLen;
  }

  uint_fast8_t
  Print::eng (double number, uint_fast8_t digits, const bool fixDP)
  {
    if (checkFloat (number) == NUM_ERRORS::Ok)
      {
        static constexpr std::array<char, 16U> engSymbols =
          { 'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm', 'k', 'M', 'G', 'T', 'P',
              'E', 'Z', 'Y' };
        static constexpr auto Symbol_k_Offset = 7U; // Position of 'k' inside of engSymbols
        static constexpr auto NumberOfEngSymbols = engSymbols.size ();
        static constexpr auto UPPER_LIM = std::pow (
            10.0, 3.0 * (NumberOfEngSymbols - Symbol_k_Offset));
        static constexpr auto LOWER_LIM = std::pow (
            10.0,
            static_cast<double> (-3 * static_cast<int> (Symbol_k_Offset + 1U)));
        if (m_isNegative)
          {
            number = -number;
          }
        if ((number < UPPER_LIM) && (number >= LOWER_LIM))
          {
            static constexpr auto EngBase = 1000U;
            auto tmpNumber = number;
            auto denom = 0;
            while (tmpNumber > static_cast<decltype(tmpNumber)> (EngBase))
              {
                tmpNumber /= static_cast<decltype(tmpNumber)> (EngBase);
                denom++;
              }
            while (tmpNumber < 1.0)
              {
                tmpNumber *= static_cast<decltype(tmpNumber)> (EngBase);
                denom--;
              }
            if (denom == 0)
              {
                if (checkFloat (number, digits) == NUM_ERRORS::Ok)
                  {
                    printFloatNumber (fixDP);
                  }
                else
                  {
                    markOutOfRange ();
                  }
              }
            else
              {
                auto offset = static_cast<decltype(denom)> (Symbol_k_Offset)
                               + (denom < 0 ? denom + 1 : denom);
                digits--;
                switch (checkFloat (tmpNumber, digits, 1))
                  {
                  case NUM_ERRORS::Overflow:
                    if (offset
                        < static_cast<decltype(offset)> (NumberOfEngSymbols - 1U))
                      {
                        offset++;
                        tmpNumber /= static_cast<decltype(tmpNumber)> (EngBase);
                        if ((checkFloat (tmpNumber, digits, 1, false)
                            == NUM_ERRORS::Ok) && (m_fracLen > 0))
                          {
                            printFloatNumber (fixDP);
                            break;
                          }
                      }
                    markOutOfRange ();
                    offset = NumberOfEngSymbols;
                    break;
                  case NUM_ERRORS::Ok:
                    {
                      printFloatNumber (fixDP);
                      break;
                    }

                  default:
                    {
                      m_intLen = 0;
                    }
                  }
#ifdef USE_GREEK_MIU_SYMBOL
                if(offset == (Symbol_k_Offset - 1U))
                {
                    show(1);
                    write(static_cast<char16_t>('μ'));
                    return m_intLen + 1;
                }
#endif
                if (offset < static_cast<decltype(offset)> (NumberOfEngSymbols))
                  {
                    *(txtNumBuf.rbegin ()) = engSymbols[offset];
                    m_intLen++;
                  }

              }
          }
        else
          {
            printSCIFloatNumber (number, fixDP);
          }
      }
    show ();
    return m_intLen;
  }

  uint_fast8_t
  Print::sci (double number, uint_fast8_t digits, const bool fixDP)
  {
    if (checkFloat (number) == NUM_ERRORS::Ok)
      {
        write( "to be implemented...");
      }
    show ();
    return m_intLen;
  }

  uint_fast8_t
  Print::println (double number, uint_fast8_t digits, const bool fixDP)
  {
    const auto n = print (number, digits, fixDP);
    writeln ();
    return n;
  }

//privates:

  template<class T, typename std::enable_if<std::is_integral<T>::value>::type* =
      nullptr>
    constexpr uint_fast8_t
    numDigits (T x)
    {
      x = x < 0 ? -x : x;

      if (x >= 10000)
        {
          if (x >= 10000000)
            {
              if (x >= 100000000)
                {
                  if (x >= 1000000000)
                    return 10U;
                  return 9U;
                }
              return 8U;
            }
          if (x >= 100000)
            {
              if (x >= 1000000)
                return 7U;
              return 6U;
            }
          return 5U;
        }
      if (x >= 100)
        {
          if (x >= 1000)
            return 4U;
          return 3U;
        }
      if (x >= 10)
        return 2U;
      return 1U;
    }

  template<class T,
      typename std::enable_if<!(std::is_integral<T>::valuet)>::type* = nullptr>
    constexpr uint_fast8_t
    numDigits (T x)
    {
      auto len = 0U;
      x = x < 0 ? -x : x;
      while (x > 0)
        {
          x /= 10U;
          len++;
        }
      return len;
    }

  template<>
    constexpr uint_fast8_t
    numDigits (unsigned char x)
    {
      if (x < 10U)
        return 1U;
      if (x < 100U)
        return 2U;
      return 3U;
    }

  template<>
    constexpr uint_fast8_t
    numDigits (char x)
    {
      if (x < 0)
        x = -x;
      return numDigits<unsigned char> (x);
    }

  NUM_ERRORS
  Print::checkFloat (double number)
  {
    m_intLen = 0;
    if (isnan (number))
      {
        write ("nan");
        return NUM_ERRORS::NotNumber;
      }
    if (isinf (number))
      {
        write ("inf");
        return NUM_ERRORS::Infinity;
      }
    m_isNegative = (number < 0.0);
    return NUM_ERRORS::Ok;
  }

  NUM_ERRORS
  Print::checkFloat (double number, uint_fast8_t usedSpace,
                     const uint_fast8_t printFrom, const bool round)
  {
    if (number
        < static_cast<decltype(number)> (std::numeric_limits<decltype(m_intPart)>::max ()))
      {
        m_printFrom = printFrom;
        m_intPart = static_cast<decltype(m_intPart)> (number);
        decltype(m_intLen) dp;
        if(m_isNegative)
        {
            m_intLen = numDigits (m_intPart) + 2U;
            dp = 1; // extra space for '-'
        }
        else
        {
            m_intLen = numDigits (m_intPart) + 1U;
            dp = 0;
        }
        usedSpace = std::min (
            usedSpace,
            static_cast<decltype(usedSpace)> (TEXT_BUFFER_SIZE - m_printFrom));
        m_fracLen = usedSpace - m_intLen;
        if (m_fracLen > -2)
          {

            uint_fast64_t dpPosition = 1U;
            if (m_fracLen > 0)
              {
                for (decltype(m_fracLen) i = 0U; i < m_fracLen; i++)
                  {
                    dpPosition *= 10U;
                  }
                dp++;
              }
            else
              {
                m_intLen--; // don't show decimal point
              }
            if (round)
              {
                number += 1.0
                    / static_cast<decltype(number)> (dpPosition << 1U);
                m_intPart = static_cast<decltype(m_intPart)>(number);
                if (m_intLen < static_cast<decltype(m_intLen)> ((numDigits (m_intPart) + dp)))
                  {
                    m_intLen++;
                    m_fracLen--;
                  }
              }
            if (m_intLen <= static_cast<decltype(m_intLen)>(usedSpace))
              {
                if (m_fracLen > 0)
                  {
                    const auto frac = number
                        - static_cast<decltype(number)> (m_intPart);
                    m_fracPart = static_cast<decltype(m_intPart)> (frac
                        * static_cast<decltype(frac)> (dpPosition));
                  }
                return NUM_ERRORS::Ok;
              }
          }
      }
    return NUM_ERRORS::Overflow;
  }

  void
  Print::printFloatNumber (bool fixDP)
  {
    auto n = m_printFrom;
    if (m_fracLen > 0)
      {
        if ((m_fracPart > 0) || fixDP)
          {
            if (fixDP)
              {
                auto rit = txtNumBuf.rbegin () + m_printFrom;
                for (auto i = 0; i < m_fracLen; i++)
                  *(rit++) = '0';
              }
            else
              {
                while ((m_fracPart % 10U) == 0U)
                  m_fracPart /= 10U;
              }
            if (m_fracPart > 0)
              {
                n += printIntegerNumber (m_fracPart, m_printFrom);
              }
            else
              {
                n += m_fracLen;
              }
            txtNumBuf[TEXT_BUFFER_SIZE - ++n] = '.';
          }
      }
    else
    {
        m_fracPart = 0;
    }
    n += printIntegerNumber (m_intPart, n);

    if (m_isNegative && ((m_intPart > 0) || (m_fracPart >0)))
      {
        txtNumBuf[TEXT_BUFFER_SIZE - ++n] = '-';
      }
    m_intLen = (n - m_printFrom);
  }

  void
  Print::printSCIFloatNumber (double number, bool fixDP)
  {
    if (number < 1.0)
      {
        m_intLen = printIntegerNumber (0);
      }
    else
      {
        markOutOfRange ();
      }
  }

  void
  Print::markOutOfRange ()
  {
    constexpr auto n = 3;
    auto end = txtNumBuf.rbegin () + n;
    std::for_each (txtNumBuf.rbegin (), end, [](auto &it)
      { it = '#';});
    m_intLen = n;
  }

}
