#ifndef FAKT_H
#define FAKT_H

#include "data_entries.h"
#include "log.h"

#include "QtWidgets\qmainwindow.h"
#include "QtSql\qsqldatabase.h"
#include "QtCore\qsettings.h"

namespace Ui {
  class fakt;
} // namespace Ui

class Fakt : public QMainWindow
{
  Q_OBJECT

public:
  explicit Fakt(QWidget *parent = nullptr);
  virtual ~Fakt();

public:
  void SetSettings(std::string const &settingsPath);

private:
  Ui::fakt *m_ui;
  QSqlDatabase m_db;
  std::string m_settingsPath;
  Settings m_settings;
  Log m_log;
};

#endif
