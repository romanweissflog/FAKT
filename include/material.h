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
  
  Data* GetData(std::string const &artNr) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
};

#endif
