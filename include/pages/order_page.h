#ifndef ORDER_PAGE_H
#define ORDER_PAGE_H

#include "page_framework.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"

#include <vector>

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
  void ReOrderPositions();
  void ReOrderIds();

private:
  void AdaptTable();

public:
  struct Data
  {
    int id;
    std::string position;
    QString number;
    QString description;
  };
  std::vector<std::pair<std::string, Data>> mapping;

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
