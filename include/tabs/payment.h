#ifndef PAYMENT_H
#define PAYMENT_H

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqlquery.h"

#include "tabs/basetab.h"

class Payment : public BaseTab
{
  Q_OBJECT
public:
  Payment(QSqlQuery &query, QWidget *parent = nullptr);
  ~Payment();

public slots:
  void HandlePayment();
};

#endif
