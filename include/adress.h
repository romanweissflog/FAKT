#ifndef ADRESS_H
#define ADRESS_H

#include "basetab.h"

class Adress : public BaseTab
{
  Q_OBJECT
public:
  Adress(QWidget *parent = nullptr);
  virtual ~Adress();

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
