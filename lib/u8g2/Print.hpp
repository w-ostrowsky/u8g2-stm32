/*
 * Print.hpp
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

#ifndef U8G2_PRINT_HPP
#define U8G2_PRINT_HPP

#include <cstddef>
#include <cinttypes>
#include <string>
#include <cmath>
#include <array>
#include <limits>

namespace Print
{

constexpr auto DEF_DIGITS = 4U;

  enum class NUM_ERRORS
    : uint_fast8_t
      {
        Ok, NotNumber, Infinity, Overflow
  };

  struct BIN
  {
    static constexpr auto value = 2U;
  };
  struct DEC
  {
    static constexpr auto value = 10U;
  };
  struct HEX
  {
    static constexpr auto value = 16U;
  };

  template<unsigned int N>
    struct HexMask
    {
      static constexpr auto  mask = HexMask<N - 1U>::mask << 4U;
      static constexpr auto value = ~(HexMask<N>::mask - 1U);
    };

  template<>
    struct HexMask<0>
    {
      static constexpr auto  mask = 1U;
    };

  constexpr uint_fast8_t TEXT_BUFFER_SIZE = 16U;

  class Print
  {

  public:
    Print () = default;
    Print (const Print&) = delete;
    Print (const Print&&) = delete;

    uint_fast8_t
    print (char);
    uint_fast8_t
    println (char);
    uint_fast8_t
    print (const char*);
    uint_fast8_t
    println (const char*);
    uint_fast8_t
    print (const std::string &);
    uint_fast8_t
    println (const std::string &);

    uint_fast8_t
    print (double, uint_fast8_t digits = DEF_DIGITS, const bool fixDP = false);
    uint_fast8_t
    println (double, uint_fast8_t digits = DEF_DIGITS, const bool fixDP = false);
    uint_fast8_t
    eng (double, uint_fast8_t digits = DEF_DIGITS, const bool fixDP = false);
    uint_fast8_t
    sci (double, uint_fast8_t digits = DEF_DIGITS, const bool fixDP = false);

    template<typename BASE = DEC, typename T,
        typename std::enable_if<
            (std::is_unsigned<T>::value) && (std::is_same<BASE, DEC>::value),
            bool>::type = true>
      uint_fast8_t
      print (T num)
      {
        m_intLen = printIntegerNumber (num);
        if (!m_correctlyCovertedToText)
          {
            markOutOfRange ();
          }
        show ();
        return m_intLen;
      }

    template<typename BASE = DEC, typename T,
        typename std::enable_if<
            (std::is_unsigned<T>::value) && (std::is_same<BASE, HEX>::value),
            bool>::type = true>
      uint_fast8_t
      print (T num)
      {
        m_intLen = printIntegerNumber<T, BASE> (static_cast<T> (num));
        if ((m_intLen > static_cast<decltype(m_intLen)> (TEXT_BUFFER_SIZE - 2U))
            || !m_correctlyCovertedToText)
          {
            markOutOfRange ();
          }
        else
          {
            auto n = (TEXT_BUFFER_SIZE - 1) - m_intLen;
            txtNumBuf[n] = 'x';
            txtNumBuf[--n] = '0';
            m_intLen += 2;
          }
        show ();
        return m_intLen;
      }

    template<typename BASE = DEC, typename T,
        typename std::enable_if<
            (!std::is_unsigned<T>::value) && (std::is_same<BASE, DEC>::value),
            bool>::type = true>
      uint_fast8_t
      print (T num)
      {
        static_assert(std::is_integral<T>::value);
        T c = (num < 0)? (-num) : num;
        using Unsigned = typename std::make_unsigned<T>::type;
        m_intLen = printIntegerNumber (static_cast<Unsigned> (c));
        if (!m_correctlyCovertedToText)
          {
            markOutOfRange ();
          }
        else if (num < 0)
          {
            if (m_intLen != TEXT_BUFFER_SIZE)
              txtNumBuf[TEXT_BUFFER_SIZE - ++m_intLen] = '-';
            else
              markOutOfRange ();
          }
        show ();
        return m_intLen;
      }

    template<typename BASE = DEC, typename T,
        typename std::enable_if<
            (!std::is_unsigned<T>::value) && (std::is_same<BASE, HEX>::value),
            bool>::type = true>
      uint_fast8_t
      print (T num)
      {
        using Unsigned_t = typename std::make_unsigned<T>::type;
        Unsigned_t c = static_cast<Unsigned_t>(num);
        if (num < 0)
          {
            static constexpr auto mask = HexMask<TEXT_BUFFER_SIZE - 3U>::value;
            c ^= mask;
            m_correctlyCovertedToText = ((num & mask) == mask);
          }
        else
          {
            m_correctlyCovertedToText = true;
          }

        if (m_correctlyCovertedToText)
          {
            m_intLen = printIntegerNumber<Unsigned_t, BASE> (
                static_cast<Unsigned_t> (c));
            if (m_correctlyCovertedToText)
              {
                auto n = (TEXT_BUFFER_SIZE) - m_intLen;
                auto padLen = static_cast<decltype(m_intLen)> ((TEXT_BUFFER_SIZE
                    - 2U) - m_intLen);
                if (num < 0)
                  {
                    m_intLen += padLen;
                    while (padLen > 0)
                      {
                        padLen--;
                        txtNumBuf[--n] = 'F';
                      }
                  }
                if (padLen < 0)
                  {
                    markOutOfRange ();
                  }
                else
                  {
                    txtNumBuf[--n] = 'x';
                    txtNumBuf[--n] = '0';
                    m_intLen += 2;
                  }
              }
            else
              {
                markOutOfRange ();
              }
          }
        else
          {
            markOutOfRange ();
          }
        show ();
        return m_intLen;
      }

    template<typename BASE = DEC, typename T, typename std::enable_if<
        (std::is_same<BASE, HEX>::value) || (std::is_same<BASE, DEC>::value),
        bool>::type = true>
      uint_fast8_t
      println (T num)
      {
        static_assert(std::is_integral<T>::value);
        const auto n = print<BASE> (num);
        writeln ();
        return n;
      }

  protected:
    virtual size_t
    write (char) = 0;
    virtual size_t
    write (char16_t) = 0;
    virtual void
    writeln () = 0;
    virtual size_t
    write (const char*) = 0;
    virtual size_t
    write (const char*, size_t) = 0;

  private:
    using Buffer_t = std::array<char, TEXT_BUFFER_SIZE>;
    Buffer_t txtNumBuf;
    bool m_correctlyCovertedToText = false;

    bool m_isNegative = false;

    uint_fast64_t m_dpPosition;
    uint_fast8_t m_printFrom;
    int_fast8_t
        m_intLen,
        m_fracLen;

    uint_fast32_t m_intPart;
    decltype(m_intPart) m_fracPart;

    NUM_ERRORS
    checkFloat (double);
    NUM_ERRORS
    checkFloat (double, uint_fast8_t, uint_fast8_t printFrom = 0U,
                bool rnd = true);
    void
    printFloatNumber (bool fixDP = false);
    void
    printSCIFloatNumber (double, bool fixDP = false);

    void
    show (const uint_fast16_t from = 0)
    {
      if ((m_intLen > 0)
          && (m_intLen <= static_cast<decltype(m_intLen)> (txtNumBuf.size ())))
        write (txtNumBuf.data () + ((txtNumBuf.size () - m_intLen) - from),
               m_intLen);
    }

    template<typename T, typename BASE = DEC>
      uint_fast8_t
      printIntegerNumber (T num, unsigned int pos = 0)
      {
        uint_fast8_t n = 0U;
        auto rit = txtNumBuf.rbegin () + pos;
        while (rit < txtNumBuf.rend ())
          {
            const auto c = num % BASE::value;
            *rit++ =
                c < ('9' - ('0' - 1U)) ? c + '0' : c + ('A' - '9' + '0' - 1U);
            n++;
            num /= BASE::value;
            if (num > 0)
              continue;
            else
              break;
          }
        m_correctlyCovertedToText = (num == 0U);
        return n;
      }
    void
    markOutOfRange ();
  };
}
#endif /* U8G2_STM32_PRINT_HPP_ */
