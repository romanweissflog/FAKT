#ifndef OFFER_PAGE_H
#define OFFER_PAGE_H

#include "general_main_page.h"

class OfferContent : public GeneralMainContent
{
  Q_OBJECT
public:
  OfferContent(Settings *settings, QString const &offerNumber, QWidget *parent = nullptr);
  ~OfferContent();

  void SetData(GeneralMainData *data) override;

public:
  OfferData *data;

private:
  QLineEdit *m_deadLineEdit;
  QLabel *m_deadLineErrorLabel;
};


class OfferPage : public PageFramework
{
  Q_OBJECT
public:
  OfferPage(Settings *settings,
    QString const &number,
    QWidget *parent = nullptr);
  ~OfferPage();

public:
  OfferContent * content;
};

#endif
