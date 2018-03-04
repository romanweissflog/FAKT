#ifndef ADDRESS_PAGE_H
#define ADDRESS_PAGE_H

#include "parent_page.h"
#include "page_framework.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class addressContent;
}

class AddressContent : public ParentPage
{
  Q_OBJECT
public:
  AddressContent(Settings *settings, QSqlQuery &query, QString const &number,
    QString const &edit = "", QWidget *parent = nullptr);
  ~AddressContent();

  void SetFocusToFirst() override;

public slots:
  void CopyData(QString);

public:
  AddressData data;         ///< internal data

private:
  Ui::addressContent *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
};


class AddressPage : public PageFramework
{
  Q_OBJECT
public:
  AddressPage(Settings *settings, QSqlQuery &query, QString const &number,
    QString const &edit = "", QWidget *parent = nullptr);
  ~AddressPage();

public:
  AddressContent *content;
};

#endif
