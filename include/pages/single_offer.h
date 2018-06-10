#ifndef SINGLE_OFFER_H
#define SINGLE_OFFER_H

#include "single_entry.h"

class SingleOffer : public SingleEntry
{
  Q_OBJECT
public:
  SingleOffer(size_t number, QWidget *parent = nullptr);

public slots:
  void EditMeta() override;

private:
  void Calculate() override;
  void Recalculate(DatabaseData const &edited) override;
};

#endif
