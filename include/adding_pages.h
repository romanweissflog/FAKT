#ifndef ADDING_PAGES_H
#define ADDING_PAGES_H

#include "data_entries.h"

#include "QtWidgets\qwidget.h"
#include "QtWidgets\qdialog.h"
#include "QtSql\qsqlquery.h"

namespace Ui
{
  class leistungPage;
  class materialPage;
}

class LeistungPage : public QDialog
{
  Q_OBJECT
public:
  LeistungPage(Settings *settings, QSqlQuery &query, QWidget *parent = nullptr);
  ~LeistungPage();
  void keyPressEvent(QKeyEvent *ev) override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  LeistungData data;

private:
  Ui::leistungPage *m_ui;
  QSqlQuery &m_query;
  double m_euroPerMin;
};

class MaterialPage : public QDialog
{
  Q_OBJECT
public:
  MaterialPage(Settings *settings, QSqlQuery &query, QWidget *parent = nullptr);
  ~MaterialPage();
  void keyPressEvent(QKeyEvent *ev) override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  MaterialData data;

private:
  Ui::materialPage *m_ui;
  QSqlQuery &m_query;
  double m_mwst;
};
#endif