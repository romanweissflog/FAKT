#ifndef SQL_HELPER_HPP_
#define SQL_HELPER_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include "QtCore\qstring.h"

struct SqlPair
{
  std::string key;
  std::string value;

  SqlPair(std::string const &key, std::string const &value)
    : key(key)
    , value(std::string("'") + value + "'")
  {}

  SqlPair(std::string const &key, double value_)
    : key(key)
  {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value_;
    value = "'" + oss.str() + "'";
  }

  SqlPair(std::string const &key, uint32_t value)
    : key(key)
    , value(std::string("'") + std::to_string(value) + "'")
  {}

  SqlPair(std::string const &key, int64_t value)
    : key(key)
    , value(std::string("'") + std::to_string(value) + "'")
  {}

  SqlPair(std::string const &key, bool value)
    : key(key)
    , value(std::string("'") + std::to_string(value) + "'")
  {}

  SqlPair(std::string const &key, QString const &value)
    : key(key)
    , value(std::string("'") + value.toStdString() + "'")
  {}
};

template<typename ...Args>
std::string GenerateInsertCommand(std::string const &table, Args... args);

template<typename ...Args>
std::string GenerateEditCommand(std::string const &table,
  std::string const &keyName,
  std::string const &key,
  Args... args);

#include "sql_helper_impl.hpp"

#endif
