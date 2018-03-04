#ifndef MATERIAL_PAGE_H
#define MATERIAL_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class materialContent;
}

class MaterialContent : public ParentPage
{
  Q_OBJECT
public:
  MaterialContent(Settings *settings, QSqlQuery &query,
    QString const &edit = "", QWidget *parent = nullptr);

  ~MaterialContent();
  void SetFocusToFirst() override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  MaterialData data;        ///< internal data

private:
  Ui::materialContent *m_ui;   ///< gui element
  QSqlQuery &m_query;       ///< database query
  double m_mwst;            ///< mwst used on every entry
  double m_hourlyRate;
};


class MaterialPage : public PageFramework
{
  Q_OBJECT
public:
  MaterialPage(Settings *settings, QSqlQuery &query,
    QString const &edit = "", QWidget *parent = nullptr);
  ~MaterialPage();

public:
  MaterialContent *content;
};

#endif
