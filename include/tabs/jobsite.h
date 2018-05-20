#ifndef JOBSITE_H
#define JOBSITE_H

#include "functionality\utils.h"
#include "basetab.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

class Jobsite : public BaseTab
{
  Q_OBJECT
public:
  Jobsite(QWidget *parent = nullptr);
  ~Jobsite();

  std::unique_ptr<Data> GetData(std::string const &artNr) override; 
  void SetData(Data *input) override;

public slots:
  void AddEntry() override;
  void EditEntry() override;

private:
  void DeleteDataTable(QString const &key) override;
};

#endif
