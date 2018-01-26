#ifndef INVOICE_PAGE_H
#define INVOICE_PAGE_H

#include "general_main_page.h"

/**
* @class Page for describing a single invoice
*/
class InvoicePage : public GeneralMainPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  InvoicePage(Settings *settings, std::string const &invoiceNumber, TabName const &tab, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
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
