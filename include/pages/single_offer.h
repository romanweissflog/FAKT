#ifndef SINGLE_OFFER_H
#define SINGLE_OFFER_H

#include "single_entry.h"

class SingleOffer : public SingleEntry
{
  Q_OBJECT
public:
  SingleOffer(size_t number, QWidget *parent = nullptr);
  void SetLastData(Data *data) override;

public:
  OfferData *data;

public slots:
  void EditMeta() override;

private:
  void Calculate() override;
  void Recalculate(Data *edited) override;
};

#endif
