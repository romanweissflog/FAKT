#ifndef MATERIAL_PAGE_H
#define MATERIAL_PAGE_H

#include "parent_page.h"
#include "page_framework.h"
#include "functionality\utils.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class materialContent;
}

class MaterialContent : public ParentPage
{
  Q_OBJECT
public:
  MaterialContent(Settings *settings,
    QString const &edit = "", QWidget *parent = nullptr);
  void SetFocusToFirst() override;

private:
  void Calculate();

signals:
  void AddPage();
  void ClosePage();

public slots:
  void CopyData(QString);
  void Copy();

public:
  CustomTable *importPage;
  DatabaseData data; 

private:
  Ui::materialContent *m_ui;
  QSqlQuery m_query;       
  double m_mwst;            
  double m_hourlyRate;
};


class MaterialPage : public PageFramework
{
  Q_OBJECT
public:
  MaterialPage(Settings *settings,
    QString const &edit = "", QWidget *parent = nullptr);
  ~MaterialPage();

public:
  MaterialContent *content;
};

#endif
