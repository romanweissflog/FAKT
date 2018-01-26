#ifndef SINGLE_INVOICE_H
#define SINGLE_INVOICE_H

#include "single_entry.h"

class SingleInvoice : public SingleEntry
{
  Q_OBJECT
public:
  SingleInvoice(size_t number, std::string const &tableName, QWidget *parent = nullptr);

  public slots:
  void EditMeta() override;

public:
  InvoiceData data;

private:
  void Calculate() override;
};

#endif
