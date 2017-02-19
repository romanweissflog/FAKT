#ifndef MATERIAL_H
#define MATERIAL_H

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qdialog.h"
#include "QtWidgets\qdialogbuttonbox.h"

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
  void EditEntry(QModelIndex);

private:
  Ui::material *m_ui;
  bool m_rc;
  QSqlDatabase m_db;
  QSqlQuery m_query;
};

#endif
