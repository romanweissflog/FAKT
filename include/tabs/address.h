#ifndef ADRESS_H
#define ADRESS_H

#include "basetab.h"

class Address : public BaseTab
{
  Q_OBJECT
public:
  Address(QWidget *parent = nullptr);

  virtual ~Address();

  std::unique_ptr<Data> GetData(std::string const &customer) override;
  void SetData(Data* data) override;

public slots:
  void AddEntry(std::optional<GeneralData> const &copyData = {}) override;
  void EditEntry() override;

private:
  void AddData(AddressData *data);
  void EditData(QString const &key, AddressData *data);
};

#endif
