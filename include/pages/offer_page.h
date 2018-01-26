#ifndef OFFER_PAGE_H
#define OFFER_PAGE_H

#include "general_main_page.h"

/**
* @class Page for describing a single offer
*/
class OfferPage : public GeneralMainPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  OfferPage(Settings *settings, std::string const &offerNumber, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~OfferPage();

  void SetData(GeneralMainData *data) override;

public:
  OfferData *data;         ///< internal data

private:
  QLineEdit *m_deadLineEdit;
  QLabel *m_deadLineErrorLabel;
};

#endif
