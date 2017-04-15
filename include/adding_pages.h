#ifndef ADDING_PAGES_H
#define ADDING_PAGES_H

#include "data_entries.h"

#include "QtWidgets\qwidget.h"
#include "QtWidgets\qdialog.h"
#include "QtSql\qsqlquery.h"

namespace Ui
{
  class servicePage;
  class materialPage;
  class adressPage;
  class generalPage;
}

class ServicePage : public QDialog
{
  Q_OBJECT
public:
  ServicePage(Settings *settings, QSqlQuery &query, QWidget *parent = nullptr);
  ~ServicePage();
  void keyPressEvent(QKeyEvent *ev) override;

private:
  void Calculate();

public slots:
  void CopyData(QString);

public:
  ServiceData data;

private:
  Ui::servicePage *m_ui;
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

class AdressPage : public QDialog
{
  Q_OBJECT
public:
  AdressPage(Settings *settings, QSqlQuery &query, 
    QString edit = "", QWidget *parent = nullptr);
  ~AdressPage();
  void keyPressEvent(QKeyEvent *ev) override;

public slots:
  void CopyData(QString);

public:
  AdressData data;

private:
  Ui::adressPage *m_ui;
  QSqlQuery &m_query;
};


class GeneralPage : public QDialog
{
  Q_OBJECT
public:
  GeneralPage(Settings *settings, QSqlQuery &query, GeneralInputData &input, QWidget *parent = nullptr);
  ~GeneralPage();
  void keyPressEvent(QKeyEvent *ev) override;

private:
  void Calculate();

public:
  GeneralData data;

private:
  Ui::generalPage *m_ui;
  QSqlQuery &m_query;
  double m_hourlyRate;
};

#endif