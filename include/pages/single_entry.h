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
  DatabaseData GetData(std::string const &artNr) override;
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
  virtual void EditMeta();

protected:
  virtual void Calculate() = 0;
  virtual void Recalculate(Data *edited);
  virtual void OnEscape();

private:
  void AddEntry(QString const &key, bool const isInserted);
  void AddData(GeneralData const &entry);
  void EditData(GeneralData const &oldEntry, GeneralData const &newEntry);
  void RemoveData(GeneralData const &entry);
  void EditAfterImport(ImportWidget *importWidget);
  void AdaptAfterInsert(QString const &key);

protected:
  TabName m_childTab;
  QSqlDatabase m_db;
  size_t m_number;                       
  std::string m_childType;
  QString m_nextKey;
  DatabaseData m_internalData;
};

#endif
