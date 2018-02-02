#ifndef SINGLE_JOBSITE_H
#define SINGLE_JOBSITE_H

#include "single_entry.h"

class SingleJobsite : public SingleEntry
{
  Q_OBJECT
public:
  SingleJobsite(size_t number, std::string const &tableName, QWidget *parent = nullptr);

public:
  InvoiceData *data;

public slots:
  void EditMeta() override;

private:
  void Calculate() override;
  void Recalculate(std::unique_ptr<Data> &edited) override;
};

#endif
