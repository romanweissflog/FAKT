#ifndef LEISTUNG_H
#define LEISTUNG_H

#include "basetab.h"

class LeistungEditEntry : public Entry
{
public:
  LeistungEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~LeistungEditEntry();

public:
  QString newValue;
};

class Leistung : public BaseTab
{
  Q_OBJECT
public:
  Leistung(QWidget *parent = nullptr);
  virtual ~Leistung();

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

private:
  std::map<std::string, bool> m_tableFilter;
};

#endif
