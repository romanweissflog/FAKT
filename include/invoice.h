#ifndef INVOICE_H
#define INVOICE_H

#include "utils.h"
#include "basetab.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

class Invoice : public BaseTab
{
  Q_OBJECT
public:
  Invoice(QWidget *parent = nullptr);
  ~Invoice();

  Data* GetData(std::string const &artNr) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;

private:
  void PrepareDoc(bool withLogo) override;
};

#endif
