#include "tabs\service.h"
#include "pages\service_page.h"
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
    TabName::ServiceTab,
    "Service",
    "LEISTUNG",
    "",
    "",
    "Leistungen",
    "ARTNR",
    printmask::Undef,
    {
      { "ARTNR", "Schl.-Nr." },
      { "HAUPTARTBEZ", "Bezeichnung" },
      { "ARTBEZ", "Extra-Information" },
      { "ME", "Einheit" },
      { "EP", "EP" },
      { "LP", "Leistung" },
      { "MP", "Material" },
      { "SP", "Hilfsmat." },
      { "BAUZEIT", "Minuten" },
      { "EKP", "EKP" }
    },
    { "ARTNR", "HAUPTARTBEZ", "ME", "EP", "LP", "MP", "SP", "BAUZEIT", "EKP" }
  };
}


Service::Service(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}

Service::~Service()
{
}

void Service::AddEntry(std::optional<GeneralData> const &copyData)
{ 
  ServicePage *page = new ServicePage(m_settings, "", this);
  if (copyData)
  {
    page->SetData(*copyData);
  }
  emit AddSubtab(page, "Leistung:Neu");
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Leistung:Neu:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Leistung:Neu:" + txt);
  });
  connect(page, &PageFramework::Accepted, [this, page]()
  {
    auto &data = page->content->data;
    Log::GetLog().Write(LogTypeInfo, m_logId, "Inside AddEntry with number " + data.key.toStdString());
    AddData(&data);
    AddAndSetLastKey(data.key);
    ShowDatabase();
    emit CloseTab("Leistung:Neu");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Leistung:Neu");
  });
}

void Service::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  ServicePage *page = new ServicePage(m_settings, schl, this);
  emit AddSubtab(page, "Leistung:Edit");
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Leistung:Edit:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Leistung:Edit:" + txt);
  });
  connect(page, &PageFramework::Accepted, [this, schl, page]()
  {
    auto &data = page->content->data;
    Log::GetLog().Write(LogTypeInfo, m_logId, "Inside EditEntry with number " + data.key.toStdString());
    EditData(schl, &data);
    EditLastKey(schl, data.key);
    ShowDatabase();
    emit CloseTab("Leistung:Edit");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Leistung:Edit");
  });
}

std::unique_ptr<Data> Service::GetData(std::string const &artNr)
{
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside GetData with number " + artNr);
  std::unique_ptr<ServiceData> data(new ServiceData);
  m_rc = m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_query.bindValue(":ID", QString::fromStdString(artNr));
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
  data->mainDescription = m_query.value(2).toString();
  data->description = m_query.value(3).toString();
  data->material = m_query.value(4).toDouble();
  data->minutes = m_query.value(5).toDouble();
  data->service = m_query.value(6).toDouble();
  data->helperMaterial = m_query.value(7).toDouble();
  data->ep = m_query.value(8).toDouble();
  data->unit = m_query.value(9).toString();
  data->ekp = m_query.value(10).toDouble();
  return data;
}

void Service::SetData(Data *input)
{
  ServiceData *data = static_cast<ServiceData*>(input);
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside SetData with number " + data->key.toStdString());
  m_rc = m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID");
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
    ShowDatabase();
  }
  else
  {
    AddData(data);
    ShowDatabase();
  }
}

void Service::AddData(ServiceData *data)
{
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside AddData with number " + data->key.toStdString());
  std::string sql = GenerateInsertCommand(tabData.tableName
    , SqlPair("ARTNR", data->key)
    , SqlPair("HAUPTARTBEZ", data->mainDescription)
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
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    QMessageBox::warning(this, tr("Hinweis"),
      tr("Leistungsnummer bereits vergeben - Eintrag wird nicht gespeichert"));
  }
}

void Service::EditData(QString const &key, ServiceData *data)
{
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside EditData with number " + key.toStdString() + " -> " + data->key.toStdString());
  std::string sql = GenerateEditCommand("LEISTUNG", "ARTNR", key.toStdString()
    , SqlPair("ARTNR", data->key)
    , SqlPair("HAUPTARTBEZ", data->mainDescription)
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
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
}
