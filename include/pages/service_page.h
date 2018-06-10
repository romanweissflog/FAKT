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
    QString const &edit = "",
    QWidget *parent = nullptr);

  ~ServiceContent();
  void SetFocusToFirst() override;
  void SetData(GeneralData const &data);

signals:
  void AddPage();
  void ClosePage();

public slots:
  void CopyData(QString);
  void Copy();

public:
  CustomTable *importPage;
  ServiceData data;  

private:
  void Calculate();

private:
  Ui::serviceContent *m_ui; 
  QSqlQuery m_query;       
  double m_euroPerMin;      
};


class ServicePage : public PageFramework
{
  Q_OBJECT
public:
  ServicePage(Settings *settings, 
    QString const &edit = "", QWidget *parent = nullptr);
  void SetData(GeneralData const &data);
  ~ServicePage();

public:
  ServiceContent *content;
};

#endif
