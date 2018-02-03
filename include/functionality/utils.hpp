#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>

namespace util
{
  template<size_t Size>
  struct TablePosNumber
  {
    int64_t integral;
    TablePosNumber<Size - 1> fractional;

    TablePosNumber(std::string const &val)
      : integral(0)
      , fractional("0")
    {
      auto getIntegralPart = [](std::string const &input) -> int32_t
      {
        auto pos = input.find(".");
        if (pos == std::string::npos)
        {
          return std::stoll(input);
        }
        return std::stoll(input.substr(0, pos));
      };
      auto getFractionalPart = [](std::string const &input) -> TablePosNumber<Size - 1>
      {
        auto pos = input.find(".");
        if (pos == std::string::npos || pos == input.size() - 1)
        {
          return TablePosNumber<Size - 1>("0");
        }
        return TablePosNumber<Size - 1>(input.substr(pos + 1, input.size() - pos - 1));
      };
      integral = getIntegralPart(val);
      fractional = getFractionalPart(val);
    }
  };

  template<>
  struct TablePosNumber<0>
  {
    int64_t integral;
    int64_t fractional;

    TablePosNumber(std::string const &val = "")
    {
      integral = std::stoll(val);
      fractional = 0;
    }
  };

  template<size_t SizeL, size_t SizeR>
  bool operator<(TablePosNumber<SizeL> const &lhs, TablePosNumber<SizeR> const &rhs)
  {
    if (lhs.integral < rhs.integral)
    {
      return true;
    }
    else if (lhs.integral > rhs.integral)
    {
      return false;
    }
    return lhs.fractional < rhs.fractional;
  }
}

#endif
