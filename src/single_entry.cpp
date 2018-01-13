#include "single_entry.h"
#include "adding_pages.h"

#include "ui_basetab.h"

#include <iostream>

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qmessagebox.h"

namespace
{
  TabData tabData
  {
    "SingleEntry",
    "BAUSTELLE",
    "RENR",
    PrintType::PrintTypeJobsite,
    {
      { "POSIT", "Pos" },
      { "ARTNR", "Art.-Nr." },
      { "ARTBEZ", "Bezeichnung" },
      { "MENGE", "Menge" },
      { "EP", "EP" },
      { "GP", "GP" },
      { "ME", "Einheit" },
      { "SP", "SP" },
      { "BAUZEIT", "Bauzeit" },
      { "P_RABATT", "Rabatt" },
      { "EKP", "EKP" },
      { "MULTI", "Aufschlag" },
      { "STUSATZ", "Stundensatz" }
    },
    { "POSIT", "ARTNR", "ARTBEZ", "MENGE", "EP", "GP" }
  };

  TabData GetTabData(std::string const &tableName, PrintType const &printType)
  {
    TabData data = tabData;
    data.tableName = "'" + tableName + "'";
    data.printType = printType;
    return data;
  }
}

SingleEntry::SingleEntry(std::string const &tableName, PrintType const &printType, 
  QWidget *parent)
  : BaseTab(GetTabData(tableName, printType), parent)
  , m_internalData(std::make_shared<GeneralMainData>())
{
  static std::string ss = "\303\237";

  this->setAttribute(Qt::WA_DeleteOnClose);

  QPushButton *importButton = new QPushButton("Import", this);
  m_ui->layoutAction->addWidget(importButton);
  connect(importButton, &QPushButton::clicked, this, &SingleEntry::ImportData);

  std::string closeString = "Schlie" + ss + "en";
  QPushButton *okButton = new QPushButton(QString::fromUtf8(closeString.c_str()), this);
  m_ui->layoutAction->addWidget(okButton);
  connect(okButton, &QPushButton::clicked, [this]()
  {
    if (m_db.isOpen())
    {
      m_db.close();
    }
    emit SaveData();
  });

  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);
}

SingleEntry::~SingleEntry()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void SingleEntry::SetDatabase(QSqlDatabase &db)
{
  m_db = db;
  m_db.open();
  m_query = QSqlQuery(m_db);

  std::string sql = "CREATE TABLE IF NOT EXISTS " + m_data.tableName
    + " (id INTEGER PRIMARY KEY, ";
  for (auto &&h : m_data.columns)
  {
    sql += h.first + " TEXT, ";
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

void SingleEntry::AddEntry()
{
  try
  {
    GeneralPage *page = new GeneralPage(m_settings, m_number, m_lastPos, m_query, this);
    page->setWindowTitle("Neuer Eintrag");
    if (page->exec() == QDialog::Accepted)
    {
      auto &entryData = page->data;
      std::string sql = GenerateInsertCommand(m_data.tableName
        , SqlPair("POSIT", entryData.pos)
        , SqlPair("ARTNR", entryData.artNr)
        , SqlPair("ARTBEZ", entryData.text)
        , SqlPair("MENGE", entryData.number)
        , SqlPair("EP", entryData.ep)
        , SqlPair("GP", entryData.total)
        , SqlPair("ME", entryData.unit)
        , SqlPair("SP", entryData.helpMat)
        , SqlPair("BAUZEIT", entryData.time)
        , SqlPair("P_RABATT", entryData.discount)
        , SqlPair("EKP", entryData.ekp)
        , SqlPair("MULTI", entryData.surcharge)
        , SqlPair("STUSATZ", entryData.hourlyRate));
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

void SingleEntry::DeleteEntry()
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
    m_query.prepare(QString::fromStdString("DELETE FROM " + m_data.tableName + " WHERE POSIT = :ID"));
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

void SingleEntry::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  GeneralPage *page = new GeneralPage(m_settings, m_number, schl.toStdString(), m_query, this);
  page->setWindowTitle("Editiere Eintrag");
  page->CopyData(m_number, schl.toStdString());
}

void SingleEntry::AddData(GeneralData const &entry)
{
  m_internalData->materialTotal += entry.material;
  m_internalData->helperTotal += entry.helpMat;
  m_internalData->serviceTotal += entry.service;
  Calculate();
}

void SingleEntry::EditData(GeneralData const &oldEntry, GeneralData const &newEntry)
{
  m_internalData->materialTotal += (newEntry.material - oldEntry.material);
  m_internalData->helperTotal += (newEntry.helpMat - oldEntry.helpMat);
  m_internalData->serviceTotal += (newEntry.service - oldEntry.service);
  Calculate();
}

void SingleEntry::RemoveData(GeneralData const &entry)
{
  m_internalData->materialTotal -= entry.material;
  m_internalData->helperTotal -= entry.helpMat;
  m_internalData->serviceTotal -= entry.service;
  Calculate();
}

void SingleEntry::ImportData()
{
  ImportWidget *import = new ImportWidget(this);
  if (import->exec() == QDialog::Accepted)
  {
    EditAfterImport(import);
    QSqlDatabase srcDb = QSqlDatabase::addDatabase("QSQLITE", "general");
    if (import->chosenTab == TabName::InvoiceTab)
    {
      srcDb.setDatabaseName("invoices.db");
    }
    else if (import->chosenTab == TabName::JobsiteTab)
    {
      srcDb.setDatabaseName("jobsites.db");
    }
    else if (import->chosenTab == TabName::OfferTab)
    {
      srcDb.setDatabaseName("offers.db");
    }
    else
    {
      throw std::runtime_error("bad general tab name");
    }
    srcDb.open();
 
    auto srcQuery = QSqlQuery(srcDb);
    std::string sql = "SELECT * FROM " + import->chosenId;   
    m_rc = srcQuery.exec(QString::fromStdString(sql));
    if (!m_rc)
    {
      qDebug() << srcQuery.lastError();
    }
    while (srcQuery.next())
    {
      GeneralData data;
      data.pos = srcQuery.value(1).toString();
      data.artNr = srcQuery.value(2).toString();
      data.text = srcQuery.value(3).toString();
      data.unit = srcQuery.value(4).toString();
      data.number = static_cast<uint32_t>(srcQuery.value(5).toDouble());
      data.ep = srcQuery.value(6).toDouble();
      data.material = srcQuery.value(7).toDouble();
      data.service = srcQuery.value(8).toDouble();
      data.helpMat = srcQuery.value(9).toDouble();
      data.total = srcQuery.value(10).toDouble();
      data.time = srcQuery.value(11).toDouble();
      data.discount = srcQuery.value(12).toDouble();
      data.surcharge = srcQuery.value(13).toDouble();
      data.hourlyRate = srcQuery.value(14).toDouble();
      data.ekp = srcQuery.value(15).toDouble();

      std::string sql = GenerateInsertCommand(m_data.tableName
        , SqlPair("POSIT", data.pos)
        , SqlPair("ARTNR", data.artNr)
        , SqlPair("ARTBEZ", data.text)
        , SqlPair("MENGE", data.number)
        , SqlPair("EP", data.ep)
        , SqlPair("GP", data.total)
        , SqlPair("ME", data.unit)
        , SqlPair("SP", data.helpMat)
        , SqlPair("BAUZEIT", data.time)
        , SqlPair("P_RABATT", data.discount)
        , SqlPair("EKP", data.ekp)
        , SqlPair("MULTI", data.surcharge)
        , SqlPair("STUSATZ", data.hourlyRate));
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

      AddData(data);
    }
    srcDb.close();
    srcDb = QSqlDatabase::database();
    srcDb.removeDatabase("general");
    ShowDatabase();
  }
}

void SingleEntry::EditAfterImport(ImportWidget *import)
{
  auto data = static_cast<GeneralMainData*>(Overwatch::GetInstance().GetTabPointer(import->chosenTab)->GetData(import->chosenId));
  if (import->importAddress)
  {
    m_internalData->customerNumber = data->customerNumber;
    m_internalData->name = data->name;
    m_internalData->place = data->place;
    m_internalData->salutation = data->salutation;
    m_internalData->street = data->street;
  }

  if (import->importEndline)
  {
    m_internalData->endline = data->endline;
  }

  if (import->importHeadline)
  {
    m_internalData->headline = data->headline;
  }

  if (import->importSubject)
  {
    m_internalData->subject = data->subject;
  }
}


SingleInvoice::SingleInvoice(std::string const &tableName, QWidget *parent)
  : SingleEntry(tableName, PrintType::PrintTypeSingleInvoice, parent)
{
  this->setWindowTitle("Rechnung");
  data.baseData = m_internalData;
}

void SingleInvoice::Calculate()
{
  data.baseData->total = data.baseData->materialTotal + data.baseData->helperTotal + data.baseData->serviceTotal;
  data.baseData->mwstTotal = data.baseData->total / 100 * data.mwst;
  data.baseData->brutto = data.baseData->total + data.baseData->mwstTotal;
  data.skontoTotal = data.baseData->brutto / 100 * data.baseData->skonto + data.baseData->brutto;
}


SingleOffer::SingleOffer(std::string const &tableName, QWidget *parent)
  : SingleEntry(tableName, PrintType::PrintTypeSingleOffer, parent)
{
  this->setWindowTitle("Angebot");
  data.baseData = m_internalData;
}

void SingleOffer::Calculate()
{
  data.baseData->total = data.baseData->materialTotal + data.baseData->helperTotal + data.baseData->serviceTotal;
  data.baseData->mwstTotal = data.baseData->total / 100 * m_settings->mwst;
  data.baseData->brutto = data.baseData->total + data.baseData->mwstTotal;
  data.baseData->skonto = data.baseData->brutto / 100 * data.baseData->skonto + data.baseData->brutto;
}


SingleJobsite::SingleJobsite(std::string const &tableName, QWidget *parent)
  : SingleEntry(tableName, PrintType::PrintTypeSingleOffer, parent)
{
  this->setWindowTitle("Baustelle");
  data.baseData = m_internalData;
}

void SingleJobsite::Calculate()
{
  data.baseData->total = data.baseData->materialTotal + data.baseData->helperTotal + data.baseData->serviceTotal;
  data.baseData->mwstTotal = data.baseData->total / 100 * data.mwst;
  data.baseData->brutto = data.baseData->total + data.baseData->mwstTotal;
  data.baseData->skonto = data.baseData->brutto / 100 * data.baseData->skonto + data.baseData->brutto;
}