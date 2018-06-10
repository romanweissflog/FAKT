#ifndef SERVICE_H
#define SERVICE_H

#include "basetab.h"

class Service : public BaseTab
{
  Q_OBJECT
public:
  Service(QWidget *parent = nullptr);
  virtual ~Service();

  std::unique_ptr<Data> GetData(std::string const &artNr) override;

  void SetData(Data *data) override;

public slots:
  void AddEntry(std::optional<GeneralData> const &copyData = {}) override;
  void EditEntry() override;

private:
  void AddData(ServiceData *data);
  void EditData(QString const &key, ServiceData *data);
};

#endif
