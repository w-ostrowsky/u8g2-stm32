
#ifndef U8G2_STM32_CONSTSTR_HPP_
#define U8G2_STM32_CONSTSTR_HPP_

#include <stdexcept>


class ConstStr {

public:
    const char* p;
    std::size_t sz;
    public:
       template<std::size_t N>
       constexpr ConstStr(const char(&a)[N]) : p(a), sz(N - 1) {}

       constexpr char operator[](std::size_t n) const
       {
           return n < sz ? p[n] : throw std::out_of_range("");
       }

       constexpr const auto length() const { return sz; }
       constexpr const auto c_str() const { return p; }
};

#endif
/* U8G2_STM32_CONSTSTR_HPP_ */
