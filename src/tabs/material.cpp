#include "tabs\material.h"
#include "pages\material_page.h"
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
    TabName::MaterialTab,
    "Material",
    "MATERIAL",
    "",
    "",
    "Material",
    "ARTNR",
    printmask::Undef,
    {
      { "ARTNR", "Schl.-Nr." },
      { "HAUPTARTBEZ", "Bezeichnung" },
      { "ARTBEZ", "Extra-Information" },
      { "ME", "Einheit" },
      { "NETTO", "Netto" },
      { "BRUTTO", "Brutto" },
      { "EKP", "EKP" },
      { "BAUZEIT", "Minuten" }
    },
    { "ARTNR", "HAUPTARTBEZ", "ME", "NETTO", "BRUTTO", "EKP", "VERARB", "BAUZEIT" }
  };
}


Material::Material(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}

Material::~Material()
{
}

void Material::AddEntry()
{
  MaterialPage *page = new MaterialPage(m_settings, "", this);
  emit AddSubtab(page, "Material:Neu");
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Material:Neu:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Material:Neu:" + txt);
  });
  connect(page, &PageFramework::Accepted, [this, page]()
  {
    auto &data = page->content->data;
    AddData(&data);
    ShowDatabase();
    emit CloseTab("Material:Neu");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Material:Neu");
  });
}

void Material::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();

  MaterialPage *page = new MaterialPage(m_settings, schl, this);
  emit AddSubtab(page, "Material:Edit");
  connect(page, &PageFramework::AddExtraPage, [this, page](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Material:Edit:" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page](QString const &txt)
  {
    emit CloseTab("Material:Edit:" + txt);
  });
  connect(page, &PageFramework::Accepted, [this, schl, page]()
  {
    auto &data = page->content->data;
    EditData(schl, &data);
    ShowDatabase();
    emit CloseTab("Material:Edit");
  });
  connect(page, &PageFramework::Declined, [&]()
  {
    emit CloseTab("Material:Edit");
  });
}

std::unique_ptr<Data> Material::GetData(std::string const &artNr)
{
  std::unique_ptr<MaterialData> data(new MaterialData());
  m_rc = m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID");
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
  data->unit = m_query.value(4).toString();
  data->ekp = m_query.value(5).toDouble();
  data->netto = m_query.value(6).toDouble();
  data->minutes = m_query.value(7).toDouble();
  data->brutto = m_query.value(8).toDouble();
  return data;
}

void Material::SetData(Data *input)
{
  MaterialData *data = static_cast<MaterialData*>(input);
  m_rc = m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID");
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

void Material::AddData(MaterialData *data)
{
  std::string sql = GenerateInsertCommand(tabData.tableName
    , SqlPair("ARTNR", data->key)
    , SqlPair("HAUPTARTBEZ", data->mainDescription)
    , SqlPair("ARTBEZ", data->description)
    , SqlPair("ME", data->unit)
    , SqlPair("NETTO", data->netto)
    , SqlPair("BRUTTO", data->brutto)
    , SqlPair("EKP", data->ekp)
    , SqlPair("BAUZEIT", data->minutes));
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
      tr("Materialnummer bereits vergeben - Eintrag wird nicht gespeichert"));
  }
}

void Material::EditData(QString const &key, MaterialData *data)
{
  std::string sql = GenerateEditCommand("MATERIAL", "ARTNR", key.toStdString()
    , SqlPair("ARTNR", data->key)
    , SqlPair("HAUPTARTBEZ", data->mainDescription)
    , SqlPair("ARTBEZ", data->description)
    , SqlPair("ME", data->unit)
    , SqlPair("NETTO", data->netto)
    , SqlPair("BRUTTO", data->brutto)
    , SqlPair("EKP", data->ekp)
    , SqlPair("BAUZEIT", data->minutes));
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
