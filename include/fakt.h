#ifndef FAKT_H
#define FAKT_H

#include "QtWidgets\qmainwindow.h"
#include "QtSql\qsqldatabase.h"

namespace Ui {
  class fakt;
} // namespace Ui

class Fakt : public QMainWindow
{
  Q_OBJECT

public:
  explicit Fakt(QWidget *parent = nullptr);
  virtual ~Fakt();

private:
  Ui::fakt *m_ui;
  QSqlDatabase m_db;
};

#endif
