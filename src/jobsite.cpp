#include "jobsite.h"
#include "adding_pages.h"
#include "single_entry.h"
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
  std::map<size_t, std::pair<std::string, std::string>> tableCols
  {
    { 0,{ "RENR", "Baust.Nr." } },
    { 1,{ "REDAT", "Datum" } },
    { 2,{ "KUNR", "Kunde" } },
    { 3,{ "NAME", "Name" } },
    { 4,{ "GESAMT", "Netto" } },
    { 5,{ "BRUTTO", "Brutto" } },
    { 6,{ "ANREDE", "Anrede" } },
    { 7,{ "STRASSE", "Straﬂe" } },
    { 8,{ "ORT", "Ort" } },
    { 9,{ "MGESAMT", "Material" } },
    { 10,{ "LGESAMT", "Leistung" } },
    { 11,{ "SGESAMT", "S-Zeug" } },
    { 12,{ "MWSTGESAMT", "MwstGesamt" } },
    { 13,{ "SKONTO", "Skonto" } },
    { 14,{ "SKBETRAG", "Skonto-Betrag" } },
    { 15,{ "BEZAHLT", "Bezahlt" } },
    { 16,{ "HEADLIN", "Header" } },
    { 17,{ "BEZADAT", "Bezahldatum" } },
    { 18,{ "LIEFDAT", "Lieferdatum" } },
    { 19,{ "Z_FRIST_N", "Zahlung normal" } },
    { 20,{ "Z_FIRST_S", "Zahlung Skonto" } },
    { 21,{ "SCHLUSS", "Schluss" } },
    { 22,{ "STUSATZ", "Stundensatz" } },
    { 23,{ "BETREFF", "Betreff" } },
    { 24,{ "MWSTSATZ", "Mwst" } }
  };
}


Jobsite::Jobsite(QWidget *parent)
  : BaseTab("Jobsite", PrintType::PrintTypeInvoice, parent)
{
  for (auto &&e : tableCols)
  {
    m_tableFilter[e.second.first] = true;
    if (e.first == 5)
    {
      break;
    }
  }
}

Jobsite::~Jobsite()
{
}

void Jobsite::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Jobsite::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      if (s.second.first.compare("REDAT") == 0)
      {
        sql += "strftime('%d.%m.%Y', REDAT), ";
      }
      else
      {
        sql += s.second.first + ", ";
      }
    }
    if (s.first == 5)
    {
      break;
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM BAUSTELLE";
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

  m_model->setQuery(m_query);
  size_t idx = 0;
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      m_model->setHeaderData((int)idx, Qt::Horizontal, QString::fromStdString(s.second.second));
      idx++;
    }
    if (s.first == 5)
    {
      break;
    }
  }
}

void Jobsite::AddEntry()
{
  std::string number = m_settings->lastJobsite;
  InvoicePage *page = new InvoicePage(m_settings, number, TabName::JobsiteTab, this);

  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = GenerateInsertCommand("BAUSTELLE"
      , SqlPair(tableCols[0].first, data.baseData->number)
      , SqlPair(tableCols[1].first, data.baseData->date)
      , SqlPair(tableCols[2].first, data.baseData->customerNumber)
      , SqlPair(tableCols[3].first, data.baseData->name)
      , SqlPair(tableCols[4].first, 0.0)
      , SqlPair(tableCols[5].first, 0.0)
      , SqlPair(tableCols[6].first, data.baseData->salutation)
      , SqlPair(tableCols[7].first, data.baseData->street)
      , SqlPair(tableCols[8].first, data.baseData->place)
      , SqlPair(tableCols[9].first, 0.0)
      , SqlPair(tableCols[10].first, 0.0)
      , SqlPair(tableCols[11].first, 0.0)
      , SqlPair(tableCols[12].first, 0.0)
      , SqlPair(tableCols[13].first, 0.0)
      , SqlPair(tableCols[14].first, 0.0)
      , SqlPair(tableCols[15].first, 0.0)
      , SqlPair(tableCols[16].first, data.baseData->headline)
      , SqlPair(tableCols[17].first, data.payDate)
      , SqlPair(tableCols[18].first, data.deliveryDate)
      , SqlPair(tableCols[19].first, data.baseData->payNormal)
      , SqlPair(tableCols[20].first, data.baseData->paySkonto)
      , SqlPair(tableCols[21].first, data.baseData->endline)
      , SqlPair(tableCols[22].first, data.baseData->hourlyRate)
      , SqlPair(tableCols[23].first, data.baseData->subject)
      , SqlPair(tableCols[24].first, data.mwst));

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
    ShowDatabase();
  }
}

void Jobsite::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString profit = m_ui->databaseView->model()->data(index.model()->index(index.row(), 2)).toString();
  std::string tableName = std::string("BA") + schl.toStdString();

  SingleInvoice *page = new SingleInvoice(tableName);
  page->SetSettings(m_settings);

  QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "jobsite");
  invoiceDb.setDatabaseName("jobsites.db");
  page->SetDatabase(invoiceDb);

  connect(page, &SingleInvoice::SaveData, [this, &invoiceDb, page, tableName]()
  {
    auto &data = page->data;
    std::string sql = GenerateEditCommand("BAUSTELLE", "RENR", data.baseData->number.toStdString()
      , SqlPair(tableCols[4].first, data.baseData->total)
      , SqlPair(tableCols[5].first, data.baseData->brutto)
      , SqlPair(tableCols[9].first, data.baseData->materialTotal)
      , SqlPair(tableCols[10].first, data.baseData->serviceTotal)
      , SqlPair(tableCols[11].first, data.baseData->helperTotal)
      , SqlPair(tableCols[12].first, data.baseData->mwstTotal)
      , SqlPair(tableCols[13].first, data.baseData->skonto)
      , SqlPair(tableCols[14].first, data.skontoTotal));

    delete page;
    invoiceDb.removeDatabase("jobsite");
  });

  connect(page, &SingleInvoice::destroyed, [&invoiceDb]()
  {
    invoiceDb.removeDatabase("jobsite");
  });

  page->show();
}

void Jobsite::DeleteEntry()
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
    m_rc = m_query.prepare("DELETE FROM BAUSTELLE WHERE RENR = :ID");
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

    QSqlDatabase invoiceDb = QSqlDatabase::addDatabase("QSQLITE", "jobsite");
    invoiceDb.setDatabaseName("jobsite.db");

    invoiceDb.open();
    QSqlQuery invoiceQuery(invoiceDb);
    m_rc = invoiceQuery.prepare("DROP TABLE IF EXISTS :ID");
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    m_query.bindValue(":ID", QString("BA") + id);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << invoiceQuery.lastError();
    }
    invoiceDb.close();
    invoiceDb = QSqlDatabase();
    invoiceDb.removeDatabase("jobsite");

    ShowDatabase();
  }
}

void Jobsite::FilterList()
{
  std::map<std::string, std::string> mapping;
  for (auto &&s : tableCols)
  {
    mapping[s.second.first] = s.second.second;
  }
  FilterTable *filter = new FilterTable(m_tableFilter, mapping, this);
  auto backup = m_tableFilter;
  int exec = filter->exec();
  if (exec == QDialog::Accepted)
  {
  }
  else
  {
    m_tableFilter = backup;
  }
  ShowDatabase();
}

void Jobsite::PrepareDoc(bool withLogo)
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM BAUSTELLE WHERE RENR = :ID");
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
  m_rc = m_query.next();
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", "jobsite");
  dataDb.setDatabaseName("jobsites.db");
  dataDb.open();
  QSqlQuery dataQuery(dataDb);

  QString sql = "SELECT * FROM BA" + id;
  m_rc = dataQuery.exec(sql);
  if (!m_rc)
  {
    qDebug() << dataQuery.lastError();
  }

  PrintData printData
  {
    "BAUSTELLE",
    m_query.value("ANREDE").toString(),
    m_query.value("NAME").toString(),
    m_query.value("STRASSE").toString(),
    m_query.value("ORT").toString(),
    m_query.value("RENR").toString(),
    m_query.value("REDAT").toString(),
    m_query.value("MWSTSATZ").toDouble(),
    m_query.value("GESAMT").toDouble(),
    m_query.value("MWSTGESAMT").toDouble(),
    m_query.value("BRUTTO").toDouble(),
    m_query.value("HEADLIN").toString(),
    m_query.value("BETREFF").toString(),
    m_query.value("SCHLUSS").toString(),
  };

  m_doc.clear();
  QTextCursor cursor(&m_doc);
  m_export(cursor, printData, dataQuery, withLogo ? m_settings->logoFile : "");
  dataDb = QSqlDatabase();
  dataDb.removeDatabase("jobsite");
}

std::vector<QString> Jobsite::GetArtNumbers()
{
  std::vector<QString> list;
  m_rc = m_query.exec("SELECT RENR FROM BAUSTELLE");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    list.push_back(m_query.value(0).toString());
  }
  return list;
}

Data* Jobsite::GetData(std::string const &artNr)
{
  GeneralMainData *data = new GeneralMainData;
  m_rc = m_query.prepare("SELECT * FROM BAUSTELLE WHERE RENR = :ID");
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }
  std::string number = artNr.substr(2, artNr.size());
  m_query.bindValue(":ID", QString::fromStdString(number));
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

  data->number = m_query.value(1).toString();
  data->date = m_query.value(2).toString();
  data->salutation = m_query.value(3).toString();
  data->name = m_query.value(4).toString();
  data->street = m_query.value(5).toString();
  data->place = m_query.value(6).toString();
  data->materialTotal = m_query.value(7).toDouble();
  data->serviceTotal = m_query.value(8).toDouble();
  data->helperTotal = m_query.value(9).toDouble();
  data->total = m_query.value(10).toDouble();
  data->mwstTotal = m_query.value(11).toDouble();
  data->brutto = m_query.value(12).toDouble();
  data->skonto = m_query.value(13).toDouble();
  data->customerNumber = m_query.value(17).toString();
  data->payNormal = m_query.value(19).toDouble();
  data->paySkonto = m_query.value(20).toDouble();
  data->endline = m_query.value(21).toString();
  data->hourlyRate = m_query.value(22).toDouble();
  data->subject = m_query.value(23).toString();
  return data;
}
