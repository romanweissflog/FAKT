#include "functionality\position.h"

Position::Position()
  : integralPart(0)
  , fractionalPart(0)
{}

Position::Position(std::string const &txt)
{
  size_t pos = txt.find(".");
  if (pos == std::string::npos || pos == txt.size() - 1)
  {
    try
    {
      integralPart = std::stol(txt);
    }
    catch (...)
    {
      integralPart = 0;
    }
    fractionalPart = 0;
  }
  else
  {
    try
    {
      integralPart = std::stol(txt.substr(0, pos));
    }
    catch (...)
    {
      integralPart = 0;
    }
    try
    {
      fractionalPart = std::stol(txt.substr(pos + 1, txt.size() - pos - 1));
    }
    catch (...)
    {
      fractionalPart = 0;
    }
  }
}

Position& Position::operator++()
{
  fractionalPart++;
  return *this;
}

std::string Position::ToString() const
{
  if (fractionalPart == 0)
  {
    return std::to_string(integralPart);
  }
  return std::to_string(integralPart) + "." + std::to_string(fractionalPart);
}

bool operator<(Position const &left, Position const &right)
{
  if (left.integralPart < right.integralPart)
  {
    return true;
  }
  if (left.integralPart > right.integralPart)
  {
    return false;
  }
  return left.fractionalPart < right.fractionalPart;
}

bool operator==(Position const &left, Position const &right)
{
  return left.integralPart == right.integralPart && left.fractionalPart == right.fractionalPart;
}

Position operator-(Position const &lhs, Position const &rhs)
{
  Position retValue;
  retValue.integralPart = lhs.integralPart - rhs.integralPart;
  retValue.fractionalPart = lhs.fractionalPart - rhs.fractionalPart;
  return retValue;
}
