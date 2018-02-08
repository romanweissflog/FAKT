#ifndef PAYMENT_PAGE_H
#define PAYMENT_PAGE_H

#include "parent_page.h"
#include "functionality\data_entries.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class paymentPage;
}

class PaymentPage : public ParentPage
{
  Q_OBJECT
public:
  PaymentPage(QSqlQuery &query, QString const &key, QWidget *parent = nullptr);
  ~PaymentPage();
  void SetFocusToFirst() override;

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  void SetData(QString const &key);
  void CalculateRest();
  void LoadOldPayments();

public:
  InvoiceData *data;
  double newPaid;

private:
  Ui::paymentPage *m_ui;
  QSqlQuery &m_query;
  double m_paidBefore;
};

#endif
