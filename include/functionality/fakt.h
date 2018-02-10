#ifndef FAKT_H
#define FAKT_H

#include "data_entries.h"
#include "log.h"
#include "backup.h"
#include "tabs\basetab.h"

#include "QtWidgets\qmainwindow.h"
#include "QtSql\qsqldatabase.h"
#include "QtCore\qsettings.h"

#include <map>

namespace Ui {
  class fakt;
} // namespace Ui

class Payment;
class SingleEntry;

class Fakt : public QMainWindow
{
  Q_OBJECT

public:
  explicit Fakt(QWidget *parent = nullptr);
  virtual ~Fakt();

public:
  void SetSettings(std::string const &settingsPath);
  void Init();

public slots:
  void AddTab(int);
  void AddSubtab(QWidget *, QString const &);
  void AddSubtab(SingleEntry *, QString const &);
  void AddSubtab(Payment *, QString const &);
  void RemoveTab(QString const &);

private:
  Ui::fakt *m_ui;
  QSqlDatabase m_db;
  Backup m_backup;
  std::string m_settingsPath;
  Settings m_settings;
  std::vector<BaseTab*> m_tabs;
  std::map<QString, int32_t> m_openTabs;
};

#endif
