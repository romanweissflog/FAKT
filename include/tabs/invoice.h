#ifndef INVOICE_H
#define INVOICE_H

#include "general.h"

class Invoice : public GeneralTab
{
  Q_OBJECT
public:
  Invoice(QWidget *parent = nullptr);

public slots:
  void OpenPayment();
};

#endif
