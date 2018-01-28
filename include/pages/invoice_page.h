#ifndef INVOICE_PAGE_H
#define INVOICE_PAGE_H

#include "general_main_page.h"

class InvoicePage : public GeneralMainPage
{
  Q_OBJECT
public:
  InvoicePage(Settings *settings, QString const &invoiceNumber, TabName const &tab, QWidget *parent = nullptr);
  ~InvoicePage();
  void SetData(GeneralMainData *data) override;

public:
  InvoiceData *data;  ///< internal data

private:
  QLineEdit *m_mwstEdit;
  QLineEdit *m_deliveryEdit;
  QLabel *m_deliveryErrorLabel;
};

#endif
