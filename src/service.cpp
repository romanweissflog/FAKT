#include "service.h"
#include "adding_pages.h"
#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"

#include <iostream>

namespace
{
  TabData tabData
  {
    "Service",
    "LEISTUNG",
    "ARTNR",
    PrintType::PrintTypeService,
    {
      { "ARTNR", "Schl.-Nr." },
      { "ARTBEZ", "Bezeichnung" },
      { "ME", "Einheit" },
      { "EP", "EP" },
      { "LP", "Leistung" },
      { "MP", "Material" },
      { "SP", "Hilfsmat." },
      { "BAUZEIT", "Minuten" },
      { "EKP", "EKP" }
    },
    { "ARTNR", "ARTBEZ", "ME", "EP", "LP", "MP", "SP", "BAUZEIT", "EKP" }
  };

  std::map<int, QString> colNames
  {
    { 0, "ARTNR" },
    { 1, "ARTBEZ" },
    { 2, "ME" },
    { 3, "EP" },
    { 4, "LP" },
    { 5, "SP" },
    { 6, "BAUZEIT" },
    { 7, "EKP" }
  };
}


Service::Service(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);
}

Service::~Service()
{
}

void Service::AddEntry()
{ 
  ServicePage *page = new ServicePage(m_settings, m_query, this);
  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    AddData(&data);
    ShowDatabase();
  }
}

void Service::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString oldValue = m_ui->databaseView->model()->data(index).toString();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  EditOneEntry *entry = new EditOneEntry(oldValue, this);
  if (entry->exec() == QDialog::Accepted)
  {
    QString col = colNames[index.column()];
    QString newValue = entry->newValue;
    QString stat = "UPDATE " + QString::fromStdString(tabData.tableName) + " SET " + col + " = '" + newValue + "' WHERE ARTNR = '" + schl + "'";
    m_rc = m_query.prepare(stat);
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }
}

void Service::DeleteEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QMessageBox *question = new QMessageBox(this);
  question->setWindowTitle("WARNUNG");
  question->setText("Wollen sie den Eintrag entfernen?");
  question->setStandardButtons(QMessageBox::Yes);
  question->addButton(QMessageBox::No);
  question->setDefaultButton(QMessageBox::No);
  if (question->exec() == QMessageBox::Yes)
  {
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM LEISTUNG WHERE ARTNR = :ID");
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_query.bindValue(":ID", id);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }
}

Data* Service::GetData(std::string const &artNr)
{
  ServiceData *data = new ServiceData;
  m_rc = m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(artNr));
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  data->key = m_query.value(1).toString();
  data->description = m_query.value(2).toString();
  data->material = m_query.value(3).toDouble();
  data->minutes = m_query.value(4).toDouble();
  data->service = m_query.value(5).toDouble();
  data->helperMaterial = m_query.value(6).toDouble();
  data->ep = m_query.value(7).toDouble();
  data->unit = m_query.value(8).toString();
  data->ekp = m_query.value(9).toDouble();
  return data;
}

void Service::SetData(Data *input)
{
  ServiceData *data = static_cast<ServiceData*>(input);
  m_rc = m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", data->key);
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.next();
  if (m_rc)
  {
    EditData(data);
  }
  else
  {
    AddData(data);
  }
}

void Service::AddData(ServiceData *data)
{
  std::string sql = GenerateInsertCommand(tabData.tableName
    , SqlPair("ARTNR", data->key)
    , SqlPair("ARTBEZ", data->description)
    , SqlPair("ME", data->unit)
    , SqlPair("EP", data->ep)
    , SqlPair("LP", data->service)
    , SqlPair("MP", data->material)
    , SqlPair("SP", data->helperMaterial)
    , SqlPair("BAUZEIT", data->minutes)
    , SqlPair("EKP", data->ekp));
  m_rc = m_query.prepare(QString::fromStdString(sql));
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
}

void Service::EditData(ServiceData *data)
{
  std::string sql = GenerateEditCommand("LEISTUNG", "ARTNR", data->key.toStdString()
    , SqlPair("ARTNR", data->key)
    , SqlPair("ARTBEZ", data->description)
    , SqlPair("ME", data->unit)
    , SqlPair("EP", data->ep)
    , SqlPair("LP", data->service)
    , SqlPair("MP", data->material)
    , SqlPair("SP", data->helperMaterial)
    , SqlPair("BAUZEIT", data->minutes)
    , SqlPair("EKP", data->ekp));
  m_rc = m_query.prepare(QString::fromStdString(sql));
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
}
