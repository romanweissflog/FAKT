#ifndef LEISTUNG_H
#define LEISTUNG_H

#include "utils.h"
#include "basetab.h"

struct LeistungEntryData
{
  QString schlNumber;
  QString descr;
  QString unit;
  double ep;
};

class LeistungEditEntry : public Entry
{
public:
  LeistungEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~LeistungEditEntry();

public:
  QLineEdit *newValue;
};

class LeistungDeleteEntry : public Entry
{
public:
  LeistungDeleteEntry(QWidget *parent = nullptr);
  virtual ~LeistungDeleteEntry();

public:
  QLineEdit *idToBeDeleted;
};

class LeistungEntry : public Entry
{
  Q_OBJECT
public:
  LeistungEntry(QWidget *parent = nullptr);
  virtual ~LeistungEntry();

public:
  LeistungEntryData data;
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
  void EditEntry(const QModelIndex &) override;
  void SearchEntry() override;
  void FilterList() override;
  void OrganizeList() override;
};

#endif
