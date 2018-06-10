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
    TabName::AddressTab,
    "Address",
    "ADRESSEN",
    "",
    "",
    "Adressen",
    "SUCHNAME",
    printmask::Undef,
    {
      { "SUCHNAME", "Suchname" },
      { "TELEFON", "Telefon" },
      { "KUNR", "K.-Nummer" },
      { "NAME", "Name" },
      { "PLZ", "PLZ" },
      { "ORT", "Ort" },
      { "STRASSE", QString::fromStdString("Stra" + german::ss + "e") },
      { "ANREDE", "Anrede" },
      { "FAX", "Fax" },
      { "EMAIL", "E-mail" }
    },
    { "SUCHNAME", "TELEFON", "KUNR", "NAME", "PLZ", "ORT", "STRASSE" }
  };
}


Address::Address(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}

Address::~Address()
{
}

void Address::AddEntry(std::optional<GeneralData> const &)
{
  QString number = QString::number(std::stoul(m_settings->lastCustomer) + 1);
  AddressPage *page = new AddressPage(m_settings, number, "", this);
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Adressen:Neu:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Adressen:Neu:" + txt);
  });
  emit AddSubtab(page, "Adressen:Neu");
  connect(page, &PageFramework::Accepted, [this, number, page]()
  {
    auto &data = page->content->data;
    AddData(&data);
    m_settings->lastCustomer = number.toStdString();
    ShowDatabase();
    emit CloseTab("Adressen:Neu");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Adressen:Neu");
  });
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

  AddressPage *page = new AddressPage(m_settings, number, schl, this);
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Adressen:Edit:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Adressen:Edit:" + txt);
  });
  emit AddSubtab(page, "Adressen:Edit");
  connect(page, &PageFramework::Accepted, [this, schl, page]()
  {
    auto &data = page->content->data;
    EditData(schl, &data);
    ShowDatabase();
    emit CloseTab("Adressen:Edit");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Adressen:Edit");
  });
}

std::unique_ptr<Data> Address::GetData(std::string const &customer)
{
  std::unique_ptr<AddressData> data(new AddressData());
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_query.bindValue(":ID", QString::fromStdString(customer));
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
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
  data->mail = m_query.value(10).toString();
  return data;
}

void Address::SetData(Data *input)
{
  AddressData *data = static_cast<AddressData*>(input);
  m_rc = m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", data->key);
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.next();
  if (m_rc)
  {
    EditData(data->key, data);
  }
  else
  {
    AddData(data);
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
    , SqlPair("FAX", data->fax)
    , SqlPair("EMAIL", data->mail));

  m_rc = m_query.prepare(QString::fromStdString(sql));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    QMessageBox::warning(this, tr("Hinweis"),
      tr("Suchname oder Nummer bereits vergeben - Eintrag wird nicht gespeichert"));
  }
}

void Address::EditData(QString const &key, AddressData *data)
{
  std::string sql = GenerateEditCommand("ADRESSEN", "SUCHNAME", key.toStdString()
    , SqlPair("SUCHNAME", data->key)
    , SqlPair("KUNR", data->number)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("NAME", data->name)
    , SqlPair("STRASSE", data->street)
    , SqlPair("PLZ", data->plz)
    , SqlPair("ORT", data->city)
    , SqlPair("TELEFON", data->phone)
    , SqlPair("FAX", data->fax)
    , SqlPair("EMAIL", data->mail));
  m_rc = m_query.prepare(QString::fromStdString(sql));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  ShowDatabase();
}
