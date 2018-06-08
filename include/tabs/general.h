#ifndef GENERAL_H
#define GENERAL_H

#include "functionality\utils.h"
#include "basetab.h"

class GeneralTab : public BaseTab
{
  Q_OBJECT
public:
  GeneralTab(TabData const &tabData, QWidget *parent = nullptr);

public slots:
  void AddEntry() override;
  void EditEntry() override;

protected:
  virtual void DeleteDataTable(QString const &key) override;
};

#endif
