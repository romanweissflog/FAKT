#ifndef SINGLE_INVOICE_H
#define SINGLE_INVOICE_H

#include "single_entry.h"

class SingleInvoice : public SingleEntry
{
  Q_OBJECT
public:
  SingleInvoice(size_t number, QWidget *parent = nullptr);
  void SetLastData(Data *data) override;

public slots:
  void EditMeta() override;
  void SummarizeData() override;

public:
  InvoiceData *data;

private:
  void Calculate() override;
  void Recalculate(Data  *edited) override;
  void EditAfterImport(ImportWidget *importWidget) override;
};

#endif
