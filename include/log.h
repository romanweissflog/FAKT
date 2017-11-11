#ifndef LOG_H_
#define LOG_H

#include <cstdint>
#include <string>
#include <fstream>

enum LogType : uint8_t
{
  LogTypeError,
  LogTypeInfo
};

class Log
{
public:
  Log(std::string const &file = "");
  ~Log();
  Log& operator=(Log const &other);
  Log& GetLog(std::string const &subType);
  void Write(LogType const &ype, std::string const &msg);

private:
  static Log *s_instance;
  std::string m_subType;
  std::ofstream m_file;
};

#endif
