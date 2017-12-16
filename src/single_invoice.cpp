#include "single_invoice.h"
#include "adding_pages.h"

#include "ui_basetab.h"

#include <iostream>

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qmessagebox.h"

namespace
{
  std::map<size_t, std::pair<std::string, std::string>> tableCols
  {
    { 0, { "POSIT", "Pos" } },
    { 1, { "ARTNR", "Art.-Nr." } },
    { 2, { "ARTBEZ", "Bezeichnung" } },
    { 3, { "MENGE", "Menge" } },
    { 4, { "EP", "EP" } },
    { 5, { "GP", "GP" } },
    { 6, { "ME", "Einheit" } },
    { 7, { "SP", "SP" } },
    { 8, { "BAUZEIT", "Bauzeit" } },
    { 9, { "P_RABATT", "Rabatt" } },
    { 10,{ "EKP", "EKP" } },
    { 11,{ "MULTI", "Aufschlag" } },
    { 12,{ "LPKORR", "Korrelation" } },
    { 13,{ "STUSATZ", "Stundensatz" } }
  };
}

SingleInvoice::SingleInvoice(std::string const &tableName, QWidget *parent)
  : BaseTab("SingleInvoice", PrintType::PrintTypeSingleInvoice, parent)
  , m_tableName("'" + tableName + "'")
{
  this->setWindowTitle("Rechnung");
  this->setAttribute(Qt::WA_DeleteOnClose);

  QPushButton *okButton = new QPushButton("Schließen", this);
  m_ui->verticalLayout->addWidget(okButton);
  connect(okButton, &QPushButton::clicked, [this]()
  {
    if (m_db.isOpen())
    {
      m_db.close();
    }
    emit SaveData();
  });

  for (auto &&e : tableCols)
  {
    m_tableFilter[e.second.first] = true;
    if (e.first == 5)
    {
      break;
    }
  }
}

SingleInvoice::~SingleInvoice()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void SingleInvoice::SetDatabase(QSqlDatabase &db)
{
  m_db = db;
  m_db.open();
  m_query = QSqlQuery(m_db);

  std::string sql = "CREATE TABLE IF NOT EXISTS " + m_tableName
    + " (id INTEGER PRIMARY KEY, ";
  for (auto &&h : tableCols)
  {
    sql += h.second.first + " TEXT, ";
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += ");";
  m_rc = m_query.exec(QString::fromStdString(sql));
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::EditEntry);

  ShowDatabase();
}

void SingleInvoice::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : tableCols)
  {
    if (m_tableFilter[s.second.first])
    {
      sql += s.second.first + ", ";
    }
    if (s.first == 5)
    {
      break;
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM " + m_tableName;
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

void SingleInvoice::AddEntry()
{
  try
  {
    GeneralPage *page = new GeneralPage(m_settings, m_number, m_lastPos, m_query, this);
    page->setWindowTitle("Neuer Eintrag");
    if (page->exec() == QDialog::Accepted)
    {
      auto &entryData = page->data;
      std::string sql = GenerateInsertCommand(m_tableName
        , SqlPair(tableCols[0].first, entryData.pos)
        , SqlPair(tableCols[1].first, entryData.artNr)
        , SqlPair(tableCols[2].first, entryData.text)
        , SqlPair(tableCols[3].first, entryData.number)
        , SqlPair(tableCols[4].first, entryData.ep)
        , SqlPair(tableCols[5].first, entryData.total)
        , SqlPair(tableCols[6].first, entryData.unit)
        , SqlPair(tableCols[7].first, entryData.helpMat)
        , SqlPair(tableCols[8].first, entryData.time)
        , SqlPair(tableCols[9].first, entryData.discount)
        , SqlPair(tableCols[10].first, entryData.ekp)
        , SqlPair(tableCols[11].first, entryData.surcharge)
        , SqlPair(tableCols[12].first, entryData.corrFactor * entryData.service)
        , SqlPair(tableCols[13].first, entryData.hourlyRate));
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
      AddData(entryData);
      ShowDatabase();
    }
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }
}

void SingleInvoice::DeleteEntry()
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
    QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    m_query.prepare(QString::fromStdString("DELETE FROM " + m_tableName + " WHERE POSIT = :ID"));
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    m_query.bindValue(":ID", schl);
    m_rc = m_query.exec();
    if (!m_rc)
    {
      qDebug() << m_query.lastError();
    }
    ShowDatabase();
  }
}

void SingleInvoice::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  GeneralPage *page = new GeneralPage(m_settings, m_number, schl.toStdString(), m_query, this);
  page->setWindowTitle("Editiere Eintrag");
  page->CopyData(m_number, schl.toStdString());
}

void SingleInvoice::FilterList()
{}

void SingleInvoice::AddData(GeneralData const &entry)
{
  data.materialTotal += entry.material;
  data.helperTotal += entry.helpMat;
  data.serviceTotal += entry.service;
  Calculate();
}

void SingleInvoice::EditData(GeneralData const &oldEntry, GeneralData const &newEntry)
{
  data.materialTotal += (newEntry.material - oldEntry.material);
  data.helperTotal += (newEntry.helpMat - oldEntry.helpMat);
  data.serviceTotal += (newEntry.service - oldEntry.service);
  Calculate();
}


void SingleInvoice::RemoveData(GeneralData const &entry)
{
  data.materialTotal -= entry.material;
  data.helperTotal -= entry.helpMat;
  data.serviceTotal -= entry.service;
  Calculate();
}

void SingleInvoice::Calculate()
{
  data.total = data.materialTotal + data.helperTotal + data.serviceTotal;
  data.mwstTotal = data.total / 100 * data.mwst;
  data.brutto = data.total + data.mwstTotal;
  data.skontoTotal = data.brutto / 100 * data.skonto + data.brutto;
}