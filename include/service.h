#ifndef SERVICE_H
#define SERVICE_H

#include "basetab.h"

class Service : public BaseTab
{
  Q_OBJECT
public:
  Service(QWidget *parent = nullptr);
  virtual ~Service();

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
