#ifndef INVOICE_PAGE_H
#define INVOICE_PAGE_H

#include "general_main_page.h"

#include <optional>

class InvoiceContent : public GeneralMainContent
{
  Q_OBJECT
public:
  InvoiceContent(Settings *settings, QString const &number, TabName const &tab, QWidget *parent = nullptr);
  void SetData(DatabaseData const &data) override;

public:
  std::optional<QString> numberForSettings;

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

  DatabaseData GetData() const override;
  void SetData(DatabaseData const &data);

private:
  void HandleBeforeAccept() override;

public:
  InvoiceContent *content;

private:
  Settings *m_settings;
  TabName m_childType;
};

#endif
