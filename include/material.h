#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

namespace Ui
{
  class material;
}

struct MaterialEntryData
{
  QString artNumber;
  QString artDescr;
  QString unit;
  double ep;
};

class MaterialEditEntry : public Entry
{
public:
  MaterialEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~MaterialEditEntry();

public:
  QLineEdit *newValue;
};

class MaterialDeleteEntry : public Entry
{
public:
  MaterialDeleteEntry(QWidget *parent = nullptr);
  virtual ~MaterialDeleteEntry();

public:
  QLineEdit *idToBeDeleted;
};

class MaterialEntry : public Entry
{
  Q_OBJECT
public:
  MaterialEntry(QWidget *parent = nullptr);
  virtual ~MaterialEntry();

public:
  MaterialEntryData data;
};

class Material : public QWidget
{
  Q_OBJECT
public:
  Material(QWidget *parent = nullptr);
  ~Material();

public slots:
  void ShowDatabase();
  void AddEntry();
  void DeleteEntry();
  void EditEntry(const QModelIndex &);

private:
  Ui::material *m_ui;
  bool m_rc;
  QSqlDatabase m_db;
  QSqlQuery m_query;
};

#endif
