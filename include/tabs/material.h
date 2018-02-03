#ifndef MATERIAL_H
#define MATERIAL_H

#include "functionality\utils.h"
#include "basetab.h"

class Material : public BaseTab
{
  Q_OBJECT
public:
  Material(QWidget *parent = nullptr);
  ~Material();
  
  std::unique_ptr<Data> GetData(std::string const &artNr) override;

  void SetData(Data *data) override;

public slots:
  void AddEntry() override;
  void EditEntry() override;

private:
  void AddData(MaterialData *data);
  void EditData(QString const &key, MaterialData *data);
};

#endif
