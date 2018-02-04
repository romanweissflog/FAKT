#ifndef SERVICE_PAGE_H
#define SERVICE_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class servicePage;
}
class ServicePage : public ParentPage
{
  Q_OBJECT
public:
  ServicePage(Settings *settings,
    QSqlQuery &query,
    QString const &edit = "",
    QWidget *parent = nullptr);

  ~ServicePage();
  void SetFocusToFirst() override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  ServiceData data;         ///< internal data

private:
  Ui::servicePage *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
  double m_euroPerMin;      ///< convertion to price
};

#endif
