#ifndef LEISTUNG_H
#define LEISTUNG_H

#include "basetab.h"

struct LeistungData
{
  QString number;
  QString description;
  QString unit;
  double ep;
  double service;
  double material;
  double helperMaterial;
  double minutes;
  double ekp;
  double total;
};

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
};

#endif
