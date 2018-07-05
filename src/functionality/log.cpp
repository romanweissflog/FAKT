#include "functionality\log.h"

#include <chrono>
#include <ctime>
#include <fstream>

namespace
{
  std::string FormattedString(std::string const &str)
  {
    std::string res = str;
    res.insert(res.end(), 11 - res.size(), ' ');
    return res;
  }

  std::string FormattedString(LogType type)
  {
    std::string str = "";
    switch (type)
    {
    case LogType::LogTypeError: str = "Error"; break;
    case LogType::LogTypeInfo:  str = "Info"; break;
    case LogType::LogTypeEvent: str = "Event"; break;
    default: str = "Unknown";
    }
    return FormattedString(str);
  }

  std::string DateString()
  {
    using namespace std::chrono;
    system_clock::time_point t = system_clock::now();
    std::time_t now = system_clock::to_time_t(t);
    std::string nowString = std::string(std::ctime(&now));
    return nowString.substr(0, nowString.size() - 1);
  }
}

Log::Log()
{}

Log::~Log()
{
}

void Log::Initialize(std::string const &file, int const &level)
{
  m_file = file;
  m_logLevel = static_cast<LogType>(level);
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

void Log::Write(LogType const &type, std::string const &instance, std::string const &msg)
{
  if (type > m_logLevel)
  {
    return;
  }

  std::lock_guard<std::mutex> lock(m_mutex);
  std::ofstream ofs(m_file.c_str(), std::ios::app);
  ofs << DateString() << " | " << FormattedString(type) 
    << " | " << FormattedString(instance) << " | " << msg << "\n";

  if (type == LogType::LogTypeError)
  {
    emit ShowMessage(QString::fromStdString(msg));
  }
}

void Log::Write(LogType const &type, size_t instance, std::string const &msg)
{
  if (type > m_logLevel)
  {
    return;
  }

  std::lock_guard<std::mutex> lock(m_mutex);
  std::ofstream ofs(m_file.c_str(), std::ios::app);
  ofs << DateString() << " | " << FormattedString(type)
    << " | " << FormattedString(m_instances[instance]) << " | " << msg << "\n";
  
  if (type == LogType::LogTypeError)
  {
    emit ShowMessage(QString::fromStdString(msg));
  }
}

bool EventLogger::eventFilter(QObject *obj, QEvent *ev)
{
  std::string msg = obj->objectName().toStdString() + " with parent ";
  auto const parent = obj->parent();
  if (parent)
  {
    msg += " with parent " + parent->objectName().toStdString();
  }
  if (ev->type() == QEvent::Type::Shortcut)
  {
    Log::GetLog().Write(LogTypeEvent, "ShortCut", msg);
  }
  else if (ev->type() == QEvent::KeyPress)
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev);
    msg += " and key " + std::to_string(keyEvent->key());
    Log::GetLog().Write(LogTypeEvent, "KeyPress", msg);
  }
  else if (ev->type() == QEvent::MouseButtonPress)
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
    msg += " and key " + std::to_string(static_cast<size_t>(mouseEvent->button()));
    Log::GetLog().Write(LogTypeEvent, "ButtonPress", msg);
  }
  return QObject::eventFilter(obj, ev);
}
