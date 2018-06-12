#ifndef SINGLE_INVOICE_H
#define SINGLE_INVOICE_H

#include "single_entry.h"

class SingleInvoice : public SingleEntry
{
  Q_OBJECT
public:
  SingleInvoice(size_t number, QWidget *parent = nullptr);

private:
  void Calculate() override;
  void Recalculate(DatabaseData const &edited) override;
};

#endif
