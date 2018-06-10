#ifndef OFFER_PAGE_H
#define OFFER_PAGE_H

#include "general_main_page.h"

#include <optional>

class OfferContent : public GeneralMainContent
{
  Q_OBJECT
public:
  OfferContent(Settings *settings, QString const &offerNumber, QWidget *parent = nullptr);

  void SetData(DatabaseData const &data) override;

public:
  std::optional<QString> numberForSettings;

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

private:
  void HandleBeforeAccept() override;

public:
  OfferContent *content;

private:
  Settings *m_settings;
};

#endif
