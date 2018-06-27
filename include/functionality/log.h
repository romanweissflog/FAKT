#ifndef LOG_H_
#define LOG_H_

#include "data_entries.h"

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

#include "qobject.h"
#include "QtCore\qdebug.h"
#include "QtGui\qevent.h"

enum LogType : uint8_t
{
  LogTypeNone = 0,
  LogTypeError = 1,
  LogTypeInfo = 2,
  LogTypeEvent = 3
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

  void Initialize(std::string const &file, int const &logLevel);
  
  static Log& GetLog();

  size_t RegisterInstance(std::string const &instance);
  void Write(LogType const &type, std::string const &instance, std::string const &msg);
  void Write(LogType const &type, size_t instance, std::string const &msg);

signals:
  void ShowMessage(QString);

private:
  Log();

private:
  std::vector<std::string> m_instances;
  LogType m_logLevel;
  std::string m_file;
  std::mutex m_mutex;
};

class EventLogger : public QObject
{
protected:
  bool eventFilter(QObject *obj, QEvent *ev) override;
};

// helper macro to avoid code duplicates

#define SHORTCUTMAP(key, slot) \
m_shortCuts[Qt::##key] = new QShortcut(QKeySequence(Qt::##key), this, SLOT(##slot())); \
m_shortCuts[Qt::##key]->setObjectName(#key); \
m_shortCuts[Qt::##key]->installEventFilter(Overwatch::GetInstance().GetEventLogger());

#define SHORTCUT(variable, key, slot) \
QShortcut *##variable = new QShortcut(QKeySequence(Qt::##key), this, SLOT(##slot())); \
##variable->setObjectName(#key); \
##variable->installEventFilter(Overwatch::GetInstance().GetEventLogger());

#define SHORTCUTSIGNAL(variable, key, signal) \
QShortcut *##variable = new QShortcut(QKeySequence(Qt::##key), this); \
connect(##variable, &QShortcut::activated, [this]() \
{ \
  ##signal; \
}); \
##variable->setObjectName(#key); \
##variable->installEventFilter(Overwatch::GetInstance().GetEventLogger());

#endif
