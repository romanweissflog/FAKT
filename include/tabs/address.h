/**
* @file address.h
*/

#ifndef ADRESS_H
#define ADRESS_H

#include "basetab.h"

/**
* @class Struct for gui address tab
*/
class Address : public BaseTab
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param parent Parent object
  */
  Address(QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  virtual ~Address();

  std::unique_ptr<Data> GetData(std::string const &customer) override;

  void SetData(std::unique_ptr<Data> &data) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;

private:
  void AddData(AddressData *data);
  void EditData(AddressData *data);
};

#endif
