#ifndef ORDER_PAGE_H
#define ORDER_PAGE_H

#include "page_framework.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"

#include <map>

namespace Ui
{
  class orderContent;
}

class OrderContent : public QWidget
{
  Q_OBJECT
public:
  OrderContent(QSqlQuery &query, QString const &table, QWidget *parent = nullptr);

public slots:
  void ReOrder();

public:
  std::map<QString, QString> mapping;

private:
  Ui::orderContent *m_ui;
};


class OrderPage : public PageFramework
{
  Q_OBJECT
public:
  OrderPage(QSqlQuery &query, QString const &table, QWidget *parent = nullptr);
  ~OrderPage();

public:
  OrderContent *content;
};

#endif
