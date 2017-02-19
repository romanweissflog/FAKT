#ifndef MATERIAL_H
#define MATERIAL_H

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

namespace Ui
{
  class material;
}

class Material : public QWidget
{
  Q_OBJECT
public:
  Material(QWidget *parent = nullptr);
  ~Material();

public slots:
  void ShowDatabase();

private:
  Ui::material *m_ui;
  bool m_rc;
  QSqlDatabase m_db;
  QSqlQuery m_query;
};

#endif
