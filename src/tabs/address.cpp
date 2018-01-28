#include "tabs\address.h"
#include "pages\address_page.h"
#include "functionality\sql_helper.hpp"

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
    "Address",
    "ADRESSEN",
    "Adressen",
    "SUCHNAME",
    PrintType::PrintTypeAddress,
    {
      { "SUCHNAME", "Suchname" },
      { "TELEFON", "Telefon" },
      { "KUNR", "K.-Nummer" },
      { "NAME", "Name" },
      { "PLZ", "PLZ" },
      { "ORT", "Ort" },
      { "STRASSE", "Stra" + german::ss + "e" },
      { "ANREDE", "Anrede" },
      { "FAX", "Fax" }
    },
    { "SUCHNAME", "TELEFON", "KUNR", "NAME", "PLZ", "ORT", "STRASSE" }
  };
}


Address::Address(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);
}

Address::~Address()
{
}

void Address::AddEntry()
{
  QString number = QString::number(std::stoul(m_settings->lastCustomer) + 1);
  AddressPage *page = new AddressPage(m_settings, m_query, number, "", this);
  page->hide();
  emit AddSubtab(page, "Adressen:Neu");
  page->setFocus();
  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    AddData(&data);
    m_settings->lastCustomer = number.toStdString();
    ShowDatabase();
  }
  emit CloseTab("Adressen:Neu");
}

void Address::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString number = m_ui->databaseView->model()->data(index.model()->index(index.row(), 2)).toString();

  AddressPage *page = new AddressPage(m_settings, m_query, number, schl, this);
  page->hide();
  emit AddSubtab(page, "Adressen:Edit");
  if (page->exec() == QDialog::Accepted)
  {
    AddressData data = page->data;
    data.key = schl;
    EditData(&data);
    ShowDatabase();
  }
  emit CloseTab("Adressen:Edit");
}

void Address::DeleteEntry()
{
  QMessageBox *question = new QMessageBox(this);
  question->setWindowTitle("WARNUNG");
  question->setText("Wollen sie den Eintrag entfernen?");
  question->setStandardButtons(QMessageBox::Yes);
  question->addButton(QMessageBox::No);
  question->setDefaultButton(QMessageBox::No);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM ADRESSEN WHERE SUCHNAME = :ID");
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

std::unique_ptr<Data> Address::GetData(std::string const &customer)
{
  std::unique_ptr<AddressData> data(new AddressData());
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(customer));
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
  data->number = m_query.value(2).toUInt();
  data->salutation = m_query.value(3).toString();
  data->name = m_query.value(4).toString();
  data->street = m_query.value(5).toString();
  data->plz = m_query.value(6).toString();
  data->city = m_query.value(7).toString();
  data->phone = m_query.value(8).toString();
  data->fax = m_query.value(9).toString();
  return data;
}

void Address::SetData(std::unique_ptr<Data> &input)
{
  std::unique_ptr<AddressData> data(static_cast<AddressData*>(input.release()));
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
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
    EditData(data.get());
  }
  else
  {
    AddData(data.get());
  }
}

void Address::AddData(AddressData *data)
{
  std::string sql = GenerateInsertCommand(std::string("ADRESSEN")
    , SqlPair("SUCHNAME", data->key)
    , SqlPair("TELEFON", data->phone)
    , SqlPair("KUNR", data->number)
    , SqlPair("NAME", data->name)
    , SqlPair("PLZ", data->plz)
    , SqlPair("ORT", data->city)
    , SqlPair("STRASSE", data->street)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("FAX", data->fax));

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

void Address::EditData(AddressData *data)
{
  std::string sql = GenerateEditCommand("ADRESSEN", "SUCHNAME", data->key.toStdString()
    , SqlPair("KUNR", data->number)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("NAME", data->name)
    , SqlPair("STRASSE", data->street)
    , SqlPair("PLZ", data->plz)
    , SqlPair("ORT", data->city)
    , SqlPair("TELEFON", data->phone)
    , SqlPair("FAX", data->fax));
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
