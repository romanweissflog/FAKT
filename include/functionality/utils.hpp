#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <cctype>

namespace util
{
  inline bool StringIsNumber(const std::string& s)
  {
    return !s.empty()
      && std::find_if(s.begin(), s.end(), [](char c)
    {
      return !std::isdigit(c);
    }) == s.end();
  }

  template<size_t Size>
  struct TablePosNumber
  {
    int64_t integral;
    TablePosNumber<Size - 1> fractional;
    bool isValid;

    TablePosNumber(std::string const &val)
      : integral(0)
      , fractional("0")
      , isValid(true)
    {
      auto getIntegralPart = [this](std::string const &input) -> int32_t
      {
        auto pos = input.find(".");
        if (pos == std::string::npos)
        {
          if (StringIsNumber(input))
          {
            return std::stoll(input);
          }
          else
          {
            isValid = false;
            return 0;
          }
        }
        auto const sub = input.substr(0, pos);
        if (StringIsNumber(sub))
        {
          return std::stoll(input.substr(0, pos));
        }
        else
        {
          isValid = false;
          return 0;
        }
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
      isValid &= fractional.isValid;
    }
  };

  template<>
  struct TablePosNumber<0>
  {
    int64_t integral;
    int64_t fractional;
    bool isValid;

    TablePosNumber(std::string const &val = "")
      : isValid(true)
    {
      if (StringIsNumber(val))
      {
        integral = std::stoll(val);
        fractional = 0;
      }
      else
      {
        isValid = false;
      }
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
