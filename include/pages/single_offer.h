#ifndef SINGLE_OFFER_H
#define SINGLE_OFFER_H

#include "single_entry.h"

class SingleOffer : public SingleEntry
{
  Q_OBJECT
public:
  SingleOffer(size_t number, std::string const &tableName, QWidget *parent = nullptr);

public:
  OfferData *data;

public slots:
  void EditMeta() override;

private:
  void Calculate() override;
  void Recalculate(std::unique_ptr<Data> &edited) override;
};

#endif
