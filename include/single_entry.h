/**
* @file single_entry.h
*/

#ifndef SINGLE_ENTRY_H
#define SINGLE_ENTRY_H

#include "basetab.h"

#include <memory>

/**
* @class class for handeling one invoice
*/
class SingleEntry : public BaseTab
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param tableName Name of the table inside database
  * @param parent The parent object
  */
  SingleEntry(std::string const &tableName, PrintType const &printType, 
    QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~SingleEntry();

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
  
  void ImportData();

protected:
  /**
  * @brief Add a single entry to the invoice
  / @param entry Entry to be added
  */
  void AddData(GeneralData const &entry);

  /**
  * @brief Edit the invoice given a single entry
  * @param oldEntry Old entry before editing
  * @param newEntry New entry after editing
  */
  void EditData(GeneralData const &oldEntry, GeneralData const &newEntry);

  /**
  * @brief Remove a single entry from the invoice
  * @param entry Entry to be removed
  */
  void RemoveData(GeneralData const &entry);

  void EditAfterImport(ImportWidget *importWidget);

  /**
  * @brief Calculate new values after an item was edited
  */
  virtual void Calculate() = 0;

protected:
  std::shared_ptr<GeneralMainData> m_internalData;  ///< internal data
  QSqlDatabase m_db;                      ///< corresponding invoice database
  int64_t m_number;                       ///< TBD
  std::string m_lastPos;                  ///< last position of this invoice
};


class SingleInvoice : public SingleEntry
{
public:
  SingleInvoice(std::string const &tableName, QWidget *parent = nullptr);

public:
  InvoiceData data;

private:
  void Calculate() override;
};


class SingleOffer : public SingleEntry
{
public:
  SingleOffer(std::string const &tableName, QWidget *parent = nullptr);

public:
  OfferData data;

private:
  void Calculate() override;
};


class SingleJobsite : public SingleEntry
{
public:
  SingleJobsite(std::string const &tableName, QWidget *parent = nullptr);

public:
  InvoiceData data;

private:
  void Calculate() override;
};

#endif