#ifndef SERVICE_H
#define SERVICE_H

#include "basetab.h"

class Service : public BaseTab
{
  Q_OBJECT
public:
  Service(QWidget *parent = nullptr);
  virtual ~Service();

  Data* GetData(std::string const &artNr) override;

  void SetData(Data *data) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;

private:
  void AddData(ServiceData *data);
  void EditData(ServiceData *data);
};

#endif
