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
  Data* GetData(std::string const &artNr) override;
  std::vector<QString> GetArtNumbers() override;

public slots:
  void ShowDatabase() override;
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void FilterList() override;
};

#endif
