#ifndef BASETAB_H
#define BASETAB_H


#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

namespace Ui
{
  class basetab;
}

class BaseTab : public QWidget
{
  Q_OBJECT
public:
  BaseTab(QWidget *parent = nullptr);
  virtual ~BaseTab();

  virtual void SetDatabase(QSqlDatabase &db);

public slots:
  virtual void ShowDatabase() = 0;
  virtual void AddEntry() = 0;
  virtual void DeleteEntry() = 0;
  virtual void EditEntry(const QModelIndex &) = 0;
  virtual void SearchEntry() = 0;
  virtual void FilterList() = 0;
  virtual void OrganizeList() = 0;

protected:
  Ui::basetab *m_ui;
  bool m_rc;
  QSqlQuery m_query;
};

#endif
