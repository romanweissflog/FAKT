#ifndef LOG_H_
#define LOG_H_

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

#include "qobject.h"

enum LogType : uint8_t
{
  LogTypeError,
  LogTypeInfo
};

class Log : public QObject
{
  Q_OBJECT
public:
  ~Log();

  Log(Log const &other) = delete;
  Log(Log &&other) = delete;
  Log operator=(Log const &other) = delete;
  Log operator=(Log const &&other) = delete;

  void Initialize(std::string const &file);
  
  static Log& GetLog();

  size_t RegisterInstance(std::string const &instance);
  void Write(LogType const &ype, size_t instance, std::string const &msg);

signals:
  void ShowMessage(QString);

private:
  Log();

private:
  std::vector<std::string> m_instances;
  std::string m_file;
  std::mutex m_mutex;
};

#endif
