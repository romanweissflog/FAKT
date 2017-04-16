#include "single_invoice.h"
#include "adding_pages.h"
#include "ui_basetab.h"

#include <iostream>

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

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

SingleInvoice::SingleInvoice(std::string const &tableName, GeneralInputData const &input, QWidget *parent)
  : BaseTab(parent)
  , m_tableName(tableName)
  , m_input(input)
{
  this->setWindowTitle("Rechnung");

  QPushButton *okButton = new QPushButton("Speichern", this);
  m_ui->verticalLayout->addWidget(okButton);
  connect(okButton, &QPushButton::clicked, [this]()
  {
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
    + "(id INTEGER PRIMARY KEY, ";
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
      m_model->setHeaderData(idx, Qt::Horizontal, QString::fromStdString(s.second.second));
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
    GeneralPage *page = new GeneralPage(m_settings, m_query, m_input, this);
    if (page->exec() == QDialog::Accepted)
    {
      auto &data = page->data;
      std::string sql = "INSERT INTO " + m_tableName + " (";
      for (auto &&s : tableCols)
      {
        sql += s.second.first + ", ";
      }
      sql = sql.substr(0, sql.size() - 2);
      sql += ") VALUES ('" + std::to_string(data.pos) + "', '" +
        data.artNr.toStdString() + "', '" +
        data.text.toStdString() + "', " +
        std::to_string(data.number) + ", " +
        std::to_string(data.ep) + ", " +
        std::to_string(data.total) + ", " +
        data.unit.toStdString() + ", '" +
        std::to_string(data.helpMat) + "', " +
        std::to_string(data.time) + ", ";
      std::to_string(data.discount) + ", " +
        std::to_string(data.ekp) + ", " +
        std::to_string(data.surcharge) + ", " +
        std::to_string(data.corrFactor*data.service) + ", " +
        std::to_string(data.hourlyRate) + ")";
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
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }
}

void SingleInvoice::DeleteEntry()
{}

void SingleInvoice::EditEntry()
{}

void SingleInvoice::FilterList()
{}