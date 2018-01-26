#ifndef LOG_H_
#define LOG_H_

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>

enum LogType : uint8_t
{
  LogTypeError,
  LogTypeInfo
};

class Log
{
public:
  Log(Log const &other) = delete;
  Log(Log &&other) = delete;
  Log operator=(Log const &other) = delete;

  void Initialize(std::string const &file);
  
  static Log& GetLog();

  size_t RegisterInstance(std::string const &instance);
  void Write(LogType const &ype, size_t instance, std::string const &msg);

private:
  Log();

private:
  std::vector<std::string> m_instances;
  std::ofstream m_file;
  std::mutex m_mutex;
};

#endif
