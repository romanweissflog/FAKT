#ifndef PAYMENT_PAGE_H
#define PAYMENT_PAGE_H

#include "parent_page.h"
#include "page_framework.h"
#include "functionality\data_entries.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class paymentContent;
}

class PaymentContent : public ParentPage
{
  Q_OBJECT
public:
  PaymentContent(QSqlQuery &query, QString const &key, QWidget *parent = nullptr);
  ~PaymentContent();
  void SetFocusToFirst() override;

private:
  void SetData(QString const &key);
  void CalculateRest();
  void LoadOldPayments();

public:
  InvoiceData *data;
  double newPaid;

private:
  Ui::paymentContent *m_ui;
  QSqlQuery &m_query;
  double m_paidBefore;
};


class PaymentPage : public PageFramework
{
  Q_OBJECT
public:
  PaymentPage(QSqlQuery &query, QString const &key, QWidget *parent = nullptr);
  ~PaymentPage();

public:
  PaymentContent * content;
};

#endif
