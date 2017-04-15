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

class SingleInvoice : public BaseTab
{
  Q_OBJECT
public:
  SingleInvoice(std::string const &tableName, QWidget *parent = nullptr);
  ~SingleInvoice();

  void SetDatabase(QSqlDatabase &db) override;

public slots:
  void ShowDatabase();
  void AddEntry();
  void DeleteEntry();
  void EditEntry();
  void FilterList();

private:
  QSqlDatabase m_db;
  std::string m_tableName;
  GeneralInputData m_input;
};

class Invoice : public BaseTab
{
  Q_OBJECT
public:
  Invoice(QWidget *parent = nullptr);
  ~Invoice();

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
