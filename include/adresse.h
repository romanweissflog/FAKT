#ifndef ADRESSE_H
#define ADRESSE_H

#include "basetab.h"

class Adresse : public BaseTab
{
  Q_OBJECT
public:
  Adresse(QWidget *parent = nullptr);
  virtual ~Adresse();

  void SetDatabase(QSqlDatabase &db);

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
