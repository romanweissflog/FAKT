#ifndef OFFER_H
#define OFFER_H

#include "utils.h"
#include "basetab.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

class Offer : public BaseTab
{
  Q_OBJECT
public:
  Offer(QWidget *parent = nullptr);
  ~Offer();

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
