#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>

namespace util
{
  inline bool IsNumber(const std::string &s)
  {
    return !s.empty() && std::all_of(s.begin(), s.end(), [](char c) -> bool
    {
      return ::isdigit((unsigned char)c);
    });
  }

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
          if (util::IsNumber(input))
          {
            return std::stoll(input);
          }
          return 0;
        }
        auto const substr = input.substr(0, pos);
        if (util::IsNumber(substr))
        {
          return std::stoll(substr);
        }
        return 0;
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
      if (util::IsNumber(val))
      {
        integral = std::stoll(val);
      }
      else
      {
        integral = 0;
      }
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
