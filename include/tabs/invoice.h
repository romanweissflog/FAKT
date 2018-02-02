#ifndef INVOICE_H
#define INVOICE_H

#include "functionality\utils.h"
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

  std::unique_ptr<Data> GetData(std::string const &artNr) override;
  void SetData(std::unique_ptr<Data> &data) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void OpenPayment();

private:
  ReturnValue PrepareDoc(bool withLogo) override;
};

#endif
