#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "basetab.h"

class Material : public BaseTab
{
  Q_OBJECT
public:
  Material(QWidget *parent = nullptr);
  ~Material();
  
  void SetDatabase(QSqlDatabase &db);
  Data* GetData(std::string const &artNr) override;
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
