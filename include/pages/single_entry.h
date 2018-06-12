#ifndef SINGLE_ENTRY_H
#define SINGLE_ENTRY_H

#include "tabs/basetab.h"

class SingleEntry : public BaseTab
{
  Q_OBJECT
public:
  SingleEntry(size_t number, 
    std::string const &prefix, 
    TabName const &childType,
    QWidget *parent = nullptr);
  ~SingleEntry();
  virtual void SetLastData(DatabaseData const &data);
  void SetDatabase(QString const &name);
  DatabaseData GetInternalData() const;

signals:
  void UpdateData();

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void ImportData();
  void InsertEntry();
  void SummarizeData();
  void CalcPercentages();
  void Order();
  void EditMeta();

protected:
  virtual void Calculate() = 0;
  virtual void Recalculate(DatabaseData const &edited);
  virtual void OnEscape();

private:
  void AddEntry(QString const &key, bool const isInserted);
  void AddData(DatabaseData const &entry);
  void EditData(DatabaseData const &oldEntry, DatabaseData const &newEntry);
  void RemoveData(DatabaseData const &entry);
  void EditAfterImport(ImportWidget *importWidget);
  void AdaptAfterInsert(QString const &key);

public:
  DatabaseData data;

protected:
  TabName m_childTab;
  QSqlDatabase m_db;
  size_t m_number;                       
  std::string m_childType;
  QString m_nextKey;
};

#endif
