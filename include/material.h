#ifndef MATERIAL_H
#define MATERIAL_H

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qdialog.h"
#include "QtWidgets\qdialogbuttonbox.h"
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

struct MaterialEditEntry : public QDialog
{
public:
  MaterialEditEntry(QString oldValue, QWidget *parent = nullptr);
  virtual ~MaterialEditEntry();

public:
  QLineEdit *newValue;

private:
  QDialogButtonBox *m_buttonBox;
  std::vector<QWidget*> m_widgets;
};

struct MaterialDeleteEntry : public QDialog
{
public:
  MaterialDeleteEntry(QWidget *parent = nullptr);
  virtual ~MaterialDeleteEntry();

public:
  QLineEdit *idToBeDeleted;

private:
  QDialogButtonBox *m_buttonBox;
  std::vector<QWidget*> m_widgets;
};

class MaterialEntry : public QDialog
{
  Q_OBJECT
public:
  MaterialEntry(QWidget *parent = nullptr);
  virtual ~MaterialEntry();

public:
  MaterialEntryData data;

private:
  std::vector<QWidget*> m_widgets;
  QDialogButtonBox *m_buttonBox;
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
