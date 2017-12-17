#ifndef JOBSITE_H
#define JOBSITE_H

#include "utils.h"
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

  void SetDatabase(QSqlDatabase &db);
  std::vector<QString> GetArtNumbers() override;
  Data* GetData(std::string const &artNr) override;

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
