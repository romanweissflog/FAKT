#include "tabs\offer.h"
#include "pages\offer_page.h"
#include "pages\single_offer.h"
#include "functionality\sql_helper.hpp"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qmessagebox.h"
#include "QtSql\qsqldriver.h"

#include <iostream>

namespace
{
  TabData tabData
  {
    "Offer",
    "ANGEBOT",
    "Angebote",
    "RENR",
    PrintType::PrintTypeOffer,
    {
      { "RENR", "Ang-.Nr." },
      { "REDAT", "Datum" },
      { "KUNR", "K.-Nr." },
      { "NAME", "Name" },
      { "GESAMT", "Netto" },
      { "BRUTTO", "Brutto" },
      { "ANREDE", "Anrede" },
      { "STRASSE", QString::fromStdString("Stra" + german::ss + "e") },
      { "ORT", "Ort" },
      { "MGESAMT", "Material" },
      { "LGESAMT", "Leistung" },
      { "SGESAMT", "Sonder" },
      { "MWSTGESAMT", "MwstGesamt" },
      { "HEADLIN", "Header" },
      { "SCHLUSS", "Schluss" },
      { "STUSATZ", "Stundensatz" },
      { "BETREFF", "Betreff" },
      { "B_FRIST", "Bindefrist" },
      { "Z_FRIST_N", "Zahlung normal" },
      { "Z_FRIST_S", "Zahlung Skonto" },
      { "SKONTO", "Skonto" }
    },
    { "RENR", "REDAT", "KUNR", "NAME", "GESAMT", "BRUTTO" }
  };
}


Offer::Offer(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}

Offer::~Offer()
{
}

void Offer::AddEntry()
{
  QString number = QString::number(std::stoul(m_settings->lastOffer) + 1);
  OfferPage *page = new OfferPage(m_settings, number, this);
  emit AddSubtab(page, "Angebote:Neu");
  connect(page, &PageFramework::Accepted, [this, page]()
  {
    auto data = page->content->data;
    std::string sql = GenerateInsertCommand("ANGEBOT"
      , SqlPair("RENR", data->number)
      , SqlPair("REDAT", data->date)
      , SqlPair("KUNR", data->customerNumber)
      , SqlPair("NAME", data->name)
      , SqlPair("GESAMT", 0.0)
      , SqlPair("BRUTTO", 0.0)
      , SqlPair("ANREDE", data->salutation)
      , SqlPair("STRASSE", data->street)
      , SqlPair("ORT", data->place)
      , SqlPair("MGESAMT", 0.0)
      , SqlPair("LGESAMT", 0.0)
      , SqlPair("SGESAMT", 0.0)
      , SqlPair("MWSTGESAMT", 0.0)
      , SqlPair("HEADLIN", data->headline)
      , SqlPair("SCHLUSS", data->endline)
      , SqlPair("STUSATZ", data->hourlyRate)
      , SqlPair("BETREFF", data->subject)
      , SqlPair("B_FRIST", data->deadLine)
      , SqlPair("Z_FRIST_N", data->payNormal)
      , SqlPair("Z_FRIST_S", data->paySkonto)
      , SqlPair("SKONTO", data->skonto));

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
        tr("Angebotsnummber bereits vergeben - Eintrag wird nicht gespeichert"));
    }
    m_settings->lastOffer = data->number.toStdString();
    ShowDatabase();
    emit CloseTab("Angebote:Neu");
  });
  connect(page, &PageFramework::Declined, [this]()
  {
    emit CloseTab("Angebote:Neu");
  });
}

void Offer::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString profit = m_ui->databaseView->model()->data(index.model()->index(index.row(), 2)).toString();
  std::string tableName = std::string("A") + schl.toStdString();

  SingleOffer *page = new SingleOffer(schl.toULongLong());
  page->SetSettings(m_settings);
  page->SetDatabase("offers.db");
  auto data = GetData(schl.toStdString());
  if (!data)
  {
    return;
  }
  page->SetLastData(data.get());

  page->hide();
  emit AddSubtab(page, "Angebote:" + schl);

  connect(page, &SingleOffer::UpdateData, [this, page, tableName]()
  {
    auto data = page->data;
    std::string sql = GenerateEditCommand("ANGEBOT", "RENR", data->number.toStdString()
      , SqlPair("GESAMT", data->total)
      , SqlPair("BRUTTO", data->brutto)
      , SqlPair("MGESAMT", data->materialTotal)
      , SqlPair("LGESAMT", data->serviceTotal)
      , SqlPair("SGESAMT", data->helperTotal)
      , SqlPair("MWSTGESAMT", data->mwstTotal));

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
  });
}

void Offer::DeleteEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_rc = m_query.prepare("DELETE FROM ANGEBOT WHERE RENR = :ID");
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
    m_query.bindValue(":ID", id);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }

    QSqlDatabase offerDb = QSqlDatabase::addDatabase("QSQLITE", "offer");
    offerDb.setDatabaseName("offers.db");

    offerDb.open();
    QSqlQuery offerQuery(offerDb);
    auto offerId = util::GetPaddedNumber(id);
    m_rc = offerQuery.exec("DROP TABLE IF EXISTS A" + offerId);
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, offerQuery.lastError().text().toStdString());
      return;
    }
    offerDb.close();
    offerDb = QSqlDatabase();
    offerDb.removeDatabase("offer");

    ShowDatabase();
  }
}

ReturnValue Offer::PrepareDoc(bool withLogo)
{
  auto const index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return ReturnValue::ReturnFailure;
  }

  QString const id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM ANGEBOT WHERE RENR = :ID");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  m_query.bindValue(":ID", id);
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }
  m_rc = m_query.next();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", "offer");
  dataDb.setDatabaseName("offers.db");
  dataDb.open();
  QSqlQuery dataQuery(dataDb);

  QString const offerId = util::GetPaddedNumber(id);
  QString sql = "SELECT * FROM A" + offerId;
  m_rc = dataQuery.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, dataQuery.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }

  PrintData printData
  {
    TabName::OfferTab,
    "ANGEBOT",
    m_query.value("ANREDE").toString(),
    m_query.value("NAME").toString(),
    m_query.value("STRASSE").toString(),
    m_query.value("ORT").toString(),
    m_query.value("RENR").toString(),
    m_query.value("REDAT").toString(),
    m_settings->mwst,
    m_query.value("GESAMT").toDouble(),
    m_query.value("MWSTGESAMT").toDouble(),
    m_query.value("BRUTTO").toDouble(),
    m_query.value("HEADLIN").toString(),
    m_query.value("BETREFF").toString(),
    m_query.value("SCHLUSS").toString(),
  };

  m_doc.clear();
  QTextCursor cursor(&m_doc);
  ReturnValue rv = m_export(cursor, printData, dataQuery, withLogo ? m_settings->logoFile : "");

  dataDb = QSqlDatabase();
  dataDb.removeDatabase("offer");

  return rv;
}

std::unique_ptr<Data> Offer::GetData(std::string const &artNr)
{
  std::unique_ptr<OfferData> data(new OfferData());
  m_rc = m_query.prepare("SELECT * FROM ANGEBOT WHERE RENR = :ID");
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

  data->number = m_query.value(1).toString();
  data->customerNumber = m_query.value(2).toString();
  data->date = m_query.value(3).toString();
  data->salutation = m_query.value(4).toString();
  data->name = m_query.value(5).toString();
  data->street = m_query.value(6).toString();
  data->place = m_query.value(7).toString();
  data->materialTotal = m_query.value(8).toDouble();
  data->serviceTotal = m_query.value(9).toDouble();
  data->helperTotal = m_query.value(10).toDouble();
  data->total = m_query.value(11).toDouble();
  data->mwstTotal = m_query.value(12).toDouble();
  data->brutto = m_query.value(13).toDouble();
  data->headline = m_query.value(14).toString();
  data->endline = m_query.value(15).toString();
  data->hourlyRate = m_query.value(16).toDouble();
  data->subject = m_query.value(17).toString();
  data->deadLine = m_query.value(18).toString();
  data->payNormal = m_query.value(19).toDouble();
  data->paySkonto = m_query.value(20).toDouble();
  data->skonto = m_query.value(21).toDouble();
  return data;
}

void Offer::SetData(Data *input)
{
  OfferData *data = static_cast<OfferData*>(input);
  std::string sql = GenerateEditCommand("ANGEBOT", "RENR", data->number.toStdString()
    , SqlPair("RENR", data->number)
    , SqlPair("REDAT", data->date)
    , SqlPair("KUNR", data->customerNumber)
    , SqlPair("NAME", data->name)
    , SqlPair("GESAMT", data->total)
    , SqlPair("BRUTTO", data->brutto)
    , SqlPair("ANREDE", data->salutation)
    , SqlPair("STRASSE", data->street)
    , SqlPair("ORT", data->place)
    , SqlPair("MGESAMT", data->materialTotal)
    , SqlPair("LGESAMT", data->serviceTotal)
    , SqlPair("SGESAMT", data->helperTotal)
    , SqlPair("MWSTGESAMT", data->mwstTotal)
    , SqlPair("HEADLIN", data->headline)
    , SqlPair("SCHLUSS", data->endline)
    , SqlPair("STUSATZ", data->hourlyRate)
    , SqlPair("BETREFF", data->subject)
    , SqlPair("B_FRIST", data->deadLine)
    , SqlPair("Z_FRIST_N", data->payNormal)
    , SqlPair("Z_FRIST_S", data->paySkonto)
    , SqlPair("SKONTO", data->skonto));

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
