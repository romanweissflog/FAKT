#ifndef SINGLE_ENTRY_H
#define SINGLE_ENTRY_H

#include "tabs/basetab.h"

class SingleEntry : public BaseTab
{
  Q_OBJECT
public:
  SingleEntry(size_t number, 
    std::string const &tableName, 
    TabName const &childType,
    QWidget *parent = nullptr);
  ~SingleEntry();
  void SetLastData(Data *data);
  std::unique_ptr<Data> GetData(std::string const &artNr) override;
  void SetDatabase(QString const &name);

signals:
  void UpdateData();

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void ImportData();
  void SummarizeData();
  virtual void EditMeta();

protected:
  virtual void Calculate() = 0;
  virtual void Recalculate(std::unique_ptr<Data> &edited);
  virtual void OnEscape();

private:
  void AddData(GeneralData const &entry);
  void EditData(GeneralData const &oldEntry, GeneralData const &newEntry);
  void RemoveData(GeneralData const &entry);
  void EditAfterImport(ImportWidget *importWidget);

protected:
  std::unique_ptr<GeneralMainData> m_internalData;
  QSqlDatabase m_db;
  size_t m_number;                       
  std::string m_childType;
};

#endif
