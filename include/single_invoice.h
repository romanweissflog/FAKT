#ifndef SINGLE_INVOICE_H
#define SINGLE_INVOICE_H

#include "basetab.h"

class SingleInvoice : public BaseTab
{
  Q_OBJECT
public:
  SingleInvoice(std::string const &tableName, GeneralInputData const &input = {}, QWidget *parent = nullptr);
  ~SingleInvoice();

  void SetDatabase(QSqlDatabase &db) override;

signals:
  void SaveData();

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

#endif