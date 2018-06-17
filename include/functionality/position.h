#ifndef POSITION_H
#define POSITION_H

#include <string>

struct Position
{
  int integralPart;
  int fractionalPart;

  Position();
  Position(std::string const &txt);
  Position(int integral, int fractional);

  Position& operator++();
  std::string ToString() const;
  friend bool operator<(Position const &left, Position const &right);
  friend bool operator==(Position const &left, Position const &right);
  friend Position operator-(Position const &left, Position const &right);
};

#endif
