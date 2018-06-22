#ifndef SINGLE_ENTRY_H
#define SINGLE_ENTRY_H

#include "tabs/basetab.h"

#include <optional>

class SingleEntry : public BaseTab
{
  Q_OBJECT
public:
  SingleEntry(size_t number, 
    std::string const &prefix, 
    TabName const &childType,
    QWidget *parent = nullptr);
  ~SingleEntry();
  virtual void SetLastData(Data *data);
  std::unique_ptr<Data> GetData(std::string const &artNr) override;
  void SetDatabase(QString const &name);

signals:
  void UpdateData();

public slots:
  void AddEntry(std::optional<GeneralData> const &copyData = {}) override;
  void DeleteEntry() override;
  void EditEntry() override;
  void ImportData();
  void InsertEntry();
  virtual void SummarizeData() = 0;
  void CalcPercentages();
  void Order();
  virtual void EditMeta();

protected:
  virtual void Calculate() = 0;
  virtual void Recalculate(Data *edited);
  virtual void OnEscape();
  virtual void AdaptPositions(QString const &table);
  virtual void EditAfterImport(ImportWidget *importWidget);
  void DoSummarizeWork(double mwst);

private:
  void AddEntry(QString const &key, bool const isInserted);
  void AddData(GeneralData const &entry);
  void EditData(GeneralData const &oldEntry, GeneralData const &newEntry);
  void RemoveData(GeneralData const &entry);
  void AdaptAfterInsert(QString const &key);

protected:
  std::unique_ptr<GeneralMainData> m_internalData;
  TabName m_childTab;
  QSqlDatabase m_db;
  size_t m_number;                       
  std::string m_childType;
  QString m_nextKey;
  std::optional<QString> m_lastMaterialImport;
  std::optional<QString> m_lastServiceImport;
};

#endif
