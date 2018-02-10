#ifndef SINGLE_JOBSITE_H
#define SINGLE_JOBSITE_H

#include "single_entry.h"

class SingleJobsite : public SingleEntry
{
  Q_OBJECT
public:
  SingleJobsite(size_t number, QWidget *parent = nullptr);
  void SetLastData(Data *data) override;

public:
  InvoiceData *data;

public slots:
  void EditMeta() override;

private:
  void Calculate() override;
  void Recalculate(Data *edited) override;
};

#endif
