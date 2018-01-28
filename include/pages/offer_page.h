#ifndef OFFER_PAGE_H
#define OFFER_PAGE_H

#include "general_main_page.h"

class OfferPage : public GeneralMainPage
{
  Q_OBJECT
public:
  OfferPage(Settings *settings, QString const &offerNumber, QWidget *parent = nullptr);
  ~OfferPage();

  void SetData(GeneralMainData *data) override;

public:
  OfferData *data;

private:
  QLineEdit *m_deadLineEdit;
  QLabel *m_deadLineErrorLabel;
};

#endif
