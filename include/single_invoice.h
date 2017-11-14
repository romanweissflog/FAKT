/**
* @file single_invoice.h
*/

#ifndef SINGLE_INVOICE_H
#define SINGLE_INVOICE_H

#include "basetab.h"

/**
* @class class for handeling one invoice
*/
class SingleInvoice : public BaseTab
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param tableName Name of the invoice inside database
  * @param parent The parent object
  */
  SingleInvoice(std::string const &tableName, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~SingleInvoice();

  /**
  * @brief Set the correspond invoice database
  * @param db  The invoice database
  */
  void SetDatabase(QSqlDatabase &db) override;

signals:
  /**
  * @brief To be clarified
  */
  void SaveData();

public slots:
  /**
  * @brief Show entire filtered database
  */
  void ShowDatabase() override;

  /**
  * @brief Add entry to current invoice
  */
  void AddEntry() override;

  /**
  * @brief Delete entry
  * @note TBD
  */
  void DeleteEntry() override;

  /**
  * @brief Edit single entry
  * @note TBD
  */
  void EditEntry() override;
  
  /**
  * @brief Show entire filtered database
  * @note TBD
  */
  void FilterList() override;

public:
  InvoiceData data;        ///< internal data

private:
  /**
  * @brief Add the invoice given a single entry
  */
  void AddData(GeneralData const &entry);

  /**
  * @brief Edit the invoice given a single entry
  */
  void EditData(GeneralData const &entry);

private:
  QSqlDatabase m_db;          ///< corresponding invoice database
  std::string m_tableName;    ///< name of the database inside main database
  int64_t m_number;           ///< TBD
  std::string m_lastPos;      ///< last position of this invoice
};

#endif