#include "offer.h"
#include "adding_pages.h"
#include "single_offer.h"
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
    { 0,{ "RENR", "Ang-.Nr." } },
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
    { 13,{ "HEADLIN", "Header" } },
    { 14,{ "SCHLUSS", "Schluss" } },
    { 15,{ "STUSATZ", "Stundensatz" } },
    { 16,{ "BETREFF", "Betreff" } },
    { 17,{ "B_FIRST", "Bindefrist" } },
    { 18,{ "Z_FRIST_N", "Zahlung normal" } },
    { 19,{ "Z_FRIST_S", "Zahlung Skonto"}},
    { 20,{ "SKONTO", "Skonto" } }
  };
}


Offer::Offer(QWidget *parent)
  : BaseTab("Offer", PrintType::PrintTypeInvoice, parent)
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

Offer::~Offer()
{
}

void Offer::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  ShowDatabase();
}

void Offer::ShowDatabase()
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
  sql += " FROM ANGEBOT";
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

void Offer::AddEntry()
{
  std::string number = m_settings->lastOffer;
  OfferPage *page = new OfferPage(m_settings, number, this);

  if (page->exec() == QDialog::Accepted)
  {
    auto &data = page->data;
    std::string sql = GenerateInsertCommand("ANGEBOT"
      , SqlPair(tableCols[0].first, data->number)
      , SqlPair(tableCols[1].first, data->date)
      , SqlPair(tableCols[2].first, data->customerNumber)
      , SqlPair(tableCols[3].first, data->name)
      , SqlPair(tableCols[4].first, 0.0)
      , SqlPair(tableCols[5].first, 0.0)
      , SqlPair(tableCols[6].first, data->salutation)
      , SqlPair(tableCols[7].first, data->street)
      , SqlPair(tableCols[8].first, data->place)
      , SqlPair(tableCols[9].first, 0.0)
      , SqlPair(tableCols[10].first, 0.0)
      , SqlPair(tableCols[11].first, 0.0)
      , SqlPair(tableCols[12].first, 0.0)
      , SqlPair(tableCols[13].first, data->headline)
      , SqlPair(tableCols[14].first, data->endline)
      , SqlPair(tableCols[15].first, data->hourlyRate)
      , SqlPair(tableCols[16].first, data->subject)
      , SqlPair(tableCols[17].first, data->deadLine)
      , SqlPair(tableCols[18].first, data->payNormal)
      , SqlPair(tableCols[19].first, data->paySkonto)
      , SqlPair(tableCols[20].first, data->skonto));

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

  SingleOffer *page = new SingleOffer(tableName);
  page->SetSettings(m_settings);

  QSqlDatabase offerDb = QSqlDatabase::addDatabase("QSQLITE", "offer");
  offerDb.setDatabaseName("offers.db");
  page->SetDatabase(offerDb);

  connect(page, &SingleOffer::SaveData, [this, &offerDb, page, tableName]()
  {
    auto &data = page->data;
    std::string sql = GenerateEditCommand("ANGEBOT", "RENR", data.number.toStdString()
      , SqlPair(tableCols[4].first, data.total)
      , SqlPair(tableCols[5].first, data.brutto)
      , SqlPair(tableCols[9].first, data.materialTotal)
      , SqlPair(tableCols[10].first, data.serviceTotal)
      , SqlPair(tableCols[11].first, data.helperTotal)
      , SqlPair(tableCols[12].first, data.mwstTotal));

    delete page;
    offerDb.removeDatabase("offer");
  });

  connect(page, &SingleOffer::destroyed, [&offerDb]()
  {
    offerDb.removeDatabase("offer");
  });

  page->show();
}

void Offer::DeleteEntry()
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
    m_rc = m_query.prepare("DELETE FROM ANGEBOT WHERE RENR = :ID");
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

    QSqlDatabase offerDb = QSqlDatabase::addDatabase("QSQLITE", "offer");
    offerDb.setDatabaseName("offers.db");

    offerDb.open();
    QSqlQuery offerQuery(offerDb);
    m_rc = offerQuery.exec("DROP TABLE IF EXISTS A" + id);
    if (!m_rc)
    {
      qDebug() << offerQuery.lastError();
    }
    offerDb.close();
    offerDb = QSqlDatabase();
    offerDb.removeDatabase("offer");

    ShowDatabase();
  }
}

void Offer::FilterList()
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

void Offer::PrepareDoc()
{
  auto index = m_ui->databaseView->currentIndex();
  QString id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  m_rc = m_query.prepare("SELECT * FROM ANGEBOT WHERE RENR = :ID");
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

  m_doc.clear();
  std::string html = "<table><tr>";
  for (auto &&s : tableCols)
  {
    html += "<th>" + s.second.second + "</th>";
  }
  html += "</tr><tr>";
  for (size_t i = 1; i < tableCols.size(); i++)
  {
    html += "<th>" + m_query.value((int)i).toString().toStdString() + "</td>";
  }
  html += "</tr></table>";
  m_doc.setHtml(QString::fromStdString(html));
}

void Offer::ExportToPDF()
{
  PrepareDoc();
  m_doc.print(&m_pdfPrinter);
}

void Offer::PrintEntry()
{
  PrepareDoc();
  BaseTab::EmitToPrinter(m_doc);
}
