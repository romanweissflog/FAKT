#ifndef RECHNUNG_H
#define RECHNUNG_H

#include "utils.h"
#include "basetab.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

struct RechnungEntryData
{
  QString schlNumber;
  QString descr;
  QString unit;
  double ep;
};

class RechnungEditEntry : public Entry
{
public:
  RechnungEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~RechnungEditEntry();

public:
  QLineEdit *newValue;
};

class RechnungDeleteEntry : public Entry
{
public:
  RechnungDeleteEntry(QWidget *parent = nullptr);
  virtual ~RechnungDeleteEntry();

public:
  QLineEdit *idToBeDeleted;
};

class RechnungEntry : public Entry
{
  Q_OBJECT
public:
  RechnungEntry(QWidget *parent = nullptr);
  virtual ~RechnungEntry();

public:
  RechnungEntryData data;
};

class Rechnung : public BaseTab
{
  Q_OBJECT
public:
  Rechnung(QWidget *parent = nullptr);
  ~Rechnung();

public slots:
  void ShowDatabase() override;
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;
  void SearchEntry() override;
  void FilterList() override;
};

#endif
