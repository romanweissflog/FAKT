#ifndef SERVICE_PAGE_H
#define SERVICE_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class serviceContent;
}
class ServiceContent : public ParentPage
{
  Q_OBJECT
public:
  ServiceContent(Settings *settings,
    QSqlQuery &query,
    QString const &edit = "",
    QWidget *parent = nullptr);

  ~ServiceContent();
  void SetFocusToFirst() override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  ServiceData data;         ///< internal data

private:
  Ui::serviceContent *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
  double m_euroPerMin;      ///< convertion to price
};


class ServicePage : public PageFramework
{
  Q_OBJECT
public:
  ServicePage(Settings *settings, QSqlQuery &query,
    QString const &edit = "", QWidget *parent = nullptr);
  ~ServicePage();

public:
  ServiceContent *content;
};

#endif
