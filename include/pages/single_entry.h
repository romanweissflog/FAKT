/**
* @file single_entry.h
*/

#ifndef SINGLE_ENTRY_H
#define SINGLE_ENTRY_H

#include "tabs/basetab.h"

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
  SingleEntry(size_t number, 
    std::string const &tableName, 
    TabName const &childType,
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

  void SetLastData(Data *data);

signals:
  /**
  * @brief To be clarified
  */
  void UpdateData();

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

  virtual void EditMeta();

protected:
  void keyPressEvent(QKeyEvent *event) override;

  /**
  * @brief Calculate new values after an item was edited
  */
  virtual void Calculate() = 0;

private:
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

protected:
  GeneralMainData *m_internalData;  ///< internal data
  QSqlDatabase m_db;                      ///< corresponding invoice database
  size_t m_number;                       
  std::string m_childType;
};

#endif
