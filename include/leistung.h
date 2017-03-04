#ifndef LEISTUNG_H
#define LEISTUNG_H

#include "basetab.h"

class Leistung : public BaseTab
{
  Q_OBJECT
public:
  Leistung(QWidget *parent = nullptr);
  virtual ~Leistung();

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
