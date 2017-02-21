#ifndef LEISTUNG_H
#define LEISTUNG_H

#include "utils.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

namespace Ui
{
  class leistung;
}

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

class Leistung : public QWidget
{
  Q_OBJECT
public:
  Leistung(QWidget *parent = nullptr);
  ~Leistung();

  void SetDatabase(QSqlDatabase &db);

public slots:
  void ShowDatabase();
  void AddEntry();
  void DeleteEntry();
  void EditEntry(const QModelIndex &);

private:
  Ui::leistung *m_ui;
  bool m_rc;
  QSqlQuery m_query;
};

#endif
