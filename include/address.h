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

  void SetDatabase(QSqlDatabase &db);
  Data* GetData(std::string const &customer) override;
  std::vector<QString> GetArtNumbers() override;

public slots:
  void ShowDatabase() override;
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void FilterList() override;
  void ExportToPDF() override;
  void PrintEntry() override;

private:
  void PrepareDoc();
};

#endif
