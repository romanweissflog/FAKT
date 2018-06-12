#include "functionality\log.h"

#include <chrono>
#include <ctime>
#include <fstream>

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

Log::Log()
{}

Log::~Log()
{
}

void Log::Initialize(std::string const &file)
{
  m_file = file;
}

Log& Log::GetLog()
{
  static Log instance;
  return instance;
}

size_t Log::RegisterInstance(std::string const &instance)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  auto entry = std::find(std::begin(m_instances), std::end(m_instances), instance);
  if (entry == std::end(m_instances))
  {
    size_t id = m_instances.size();
    m_instances.push_back(instance);
    return id;
  }
  return std::distance(std::begin(m_instances), entry);
}

void Log::Write(LogType const &type, size_t instance, std::string const &msg)
{
  using namespace std::chrono;

  std::lock_guard<std::mutex> lock(m_mutex);
  std::ofstream ofs(m_file.c_str(), std::ios::app);
  system_clock::time_point t = system_clock::now();
  std::time_t now = system_clock::to_time_t(t);
  std::string nowString = std::string(std::ctime(&now));
  std::string formatted = nowString.substr(0, nowString.size() - 1);
  ofs << formatted << " | " << to_string(type)
    << " | " << m_instances[instance] << " | " << msg << "\n";

  emit ShowMessage(QString::fromStdString(msg));
}
