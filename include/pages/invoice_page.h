#ifndef INVOICE_PAGE_H
#define INVOICE_PAGE_H

#include "general_main_page.h"

class InvoiceContent : public GeneralMainContent
{
  Q_OBJECT
public:
  InvoiceContent(Settings *settings, QString const &invoiceNumber, TabName const &tab, QWidget *parent = nullptr);
  ~InvoiceContent();
  void SetData(GeneralMainData *data) override;

public:
  InvoiceData *data;  ///< internal data

private:
  QLineEdit *m_mwstEdit;
  QLineEdit *m_deliveryEdit;
  QLabel *m_deliveryErrorLabel;
};


class InvoicePage : public PageFramework
{
  Q_OBJECT
public:
  InvoicePage(Settings *settings,
    QString const &number,
    TabName const &childType,
    QWidget *parent = nullptr);
  ~InvoicePage();

public:
  InvoiceContent * content;
};

#endif
