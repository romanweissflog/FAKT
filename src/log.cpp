#include "log.h"

#include <chrono>
#include <ctime>

namespace
{
  std::string to_string(LogType type)
  {
    switch (type)
    {
    case LogType::LogTypeError: return "Error";
    case LogType::LogTypeInfo: return "Info";
    default: return "Unknown";
    }
    return "Unknown";
  }
}

Log* Log::s_instance = nullptr;

Log::Log(std::string const &file)
{
  if (file.size() != 0)
  {
    m_file.open(file.c_str(), std::ios::app);
  }
}

Log& Log::operator=(Log const &other)
{
  return *this;
}

Log::~Log()
{
  if (m_file.is_open())
  {
    m_file.close();
  }
}

Log& Log::GetLog(std::string const &subType)
{
  return *s_instance;
}

void Log::Write(LogType const &type, std::string const &msg)
{
  using namespace std::chrono;
  system_clock::time_point t = system_clock::now();
  std::time_t now = system_clock::to_time_t(t);
  m_file << std::ctime(&now) << " | " << to_string(type)
    << " | " << m_subType << " | " << msg << "\n";
}