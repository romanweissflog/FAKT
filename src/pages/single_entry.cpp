#include "pages\single_entry.h"
#include "pages\general_page.h"
#include "functionality\sql_helper.hpp"
#include "functionality\overwatch.h"

#include "ui_basetab.h"

#include <iostream>

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qmessagebox.h"
#include "QtWidgets\qshortcut.h"
#include "QtGui\qevent.h"

namespace
{
  TabData tabData
  {
    "SingleEntry",
    "",
    "",
    "POSIT",
    PrintType::PrintTypeUndef,
    {
      { "POSIT", "Pos" },
      { "ARTNR", "Art.-Nr." },
      { "ARTBEZ", "Bezeichnung" },
      { "MENGE", "Menge" },
      { "EP", "EP" },
      { "GP", "GP" },
      { "ME", "Einheit" },
      { "SP", "Sonder" },
      { "BAUZEIT", "Bauzeit" },
      { "P_RABATT", "Rabatt" },
      { "EKP", "EKP" },
      { "MULTI", "Aufschlag" },
      { "STUSATZ", "Stundensatz" }
    },
    { "POSIT", "ARTNR", "ARTBEZ", "MENGE", "EP", "GP" }
  };

  TabData GetTabData(std::string const &tableName)
  {
    TabData data = tabData;
    data.tableName = "'" + tableName + "'";
    return data;
  }

  double GetMaterialPrice(GeneralData const &data)
  {
    double matPriceSurcharge = (100.0 + data.surcharge) / 100.0 * data.ekp;
    double matPrice = (100.0 - data.discount) / 100.0 * matPriceSurcharge;
    return data.number * matPrice;
  }

  double GetServicePrice(GeneralData const &data)
  {
    double servicePrice = data.time / 60.0 * data.hourlyRate;
    return data.number * servicePrice;
  }

  double GetHelperPrice(GeneralData const &data)
  {
    return data.number * data.helpMat;
  }
}

SingleEntry::SingleEntry(size_t number, 
  std::string const &tableName,
  TabName const &childType, 
  QWidget *parent)
  : BaseTab(GetTabData(tableName), parent)
  , m_number(number)
{
  this->setAttribute(Qt::WA_DeleteOnClose);

  if (childType == TabName::OfferTab)
  {
    m_internalData.reset(new OfferData());
    m_childType = "Angebot";
  }
  else
  {
    m_internalData.reset(new InvoiceData());
    if (childType == TabName::InvoiceTab)
    {
      m_childType = "Rechnung";
    }
    else
    {
      m_childType = "Baustelle";
    }
  }

  QPushButton *editMeta = new QPushButton(QString::fromStdString("Allgemein (G)"), this);
  m_ui->layoutAction->addWidget(editMeta);
  connect(editMeta, &QPushButton::clicked, this, &SingleEntry::EditMeta);

  QPushButton *importButton = new QPushButton("Import (I)", this);
  m_ui->layoutAction->addWidget(importButton);
  connect(importButton, &QPushButton::clicked, this, &SingleEntry::ImportData);

  m_internalData->number = QString::number(m_number);

  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  new QShortcut(QKeySequence(Qt::Key_G), this, SLOT(EditMeta()));
  new QShortcut(QKeySequence(Qt::Key_I), this, SLOT(ImportData()));
}

SingleEntry::~SingleEntry()
{
  if (m_db.isOpen())
  {
    m_db.close();
  }
}

void SingleEntry::SetDatabase(QString const &name)
{
  m_db = QSqlDatabase::addDatabase("QSQLITE", "general_" + m_data.tabName);
  m_db.setDatabaseName(name);
  m_db.open();
  m_query = QSqlQuery(m_db);

  std::string sql = "CREATE TABLE IF NOT EXISTS " + m_data.tableName
    + " (id INTEGER PRIMARY KEY, ";
  for (auto &&h : m_data.columns)
  {
    if (h.first == "POSIT")
    {
      sql += h.first + " TEXT UNIQUE, ";
    }
    else
    {
      sql += h.first + " TEXT, ";
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += ");";
  m_rc = m_query.exec(QString::fromStdString(sql));
  if (!m_rc)
  {
    qDebug() << m_query.lastError();
  }

  ShowDatabase();
}

void SingleEntry::AddEntry()
{
  try
  {
    GeneralPage *page = new GeneralPage(m_settings, m_number, m_childType, m_query, this);
    page->setWindowTitle("Neuer Eintrag");
    page->hide();
    QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Neu";
    emit AddSubtab(page, tabName);
    page->setFocus();
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
        QMessageBox::warning(this, tr("Warnung"),
          tr("Position exisiert bereits"));
      }
      else
      {
        AddData(entryData);
      }
      ShowDatabase();
    }
    emit CloseTab(tabName);
  }
  catch (std::runtime_error e)
  {
    std::cout << e.what() << std::endl;
  }
}

void SingleEntry::DeleteEntry()
{
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    std::unique_ptr<GeneralData> entry(static_cast<GeneralData*>(GetData(schl.toStdString()).release()));
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
    RemoveData(*entry);
    ShowDatabase();
  }
}

void SingleEntry::EditEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  GeneralPage *page = new GeneralPage(m_settings, m_number, m_data.type, m_query, this);
  page->setWindowTitle("Editiere Eintrag");
  std::unique_ptr<GeneralData> oldData(static_cast<GeneralData*>(GetData(schl.toStdString()).release()));
  page->CopyData(oldData.get());

  page->hide();
  QString tabName = m_data.tabName + ":" + QString::number(m_number) + ":Edit";
  emit AddSubtab(page, tabName);
  page->setFocus();

  if (page->exec() == QDialog::Accepted)
  {
    auto &entryData = page->data;
    std::string sql = GenerateEditCommand(m_data.tableName, m_data.idString.toStdString(), entryData.pos.toStdString()
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
    EditData(*oldData, entryData);
    ShowDatabase();
  }
  emit CloseTab(tabName);
}

void SingleEntry::SetLastData(Data *input)
{
  GeneralMainData *data = static_cast<GeneralMainData*>(input);
  m_internalData->brutto = data->brutto;
  m_internalData->helperTotal = data->helperTotal;
  m_internalData->materialTotal = data->materialTotal;
  m_internalData->mwstTotal = data->mwstTotal;
  m_internalData->serviceTotal = data->serviceTotal;
  m_internalData->total = data->total;
}

void SingleEntry::AddData(GeneralData const &entry)
{
  m_internalData->materialTotal += GetMaterialPrice(entry);
  m_internalData->helperTotal += GetHelperPrice(entry);
  m_internalData->serviceTotal += GetServicePrice(entry);
  Calculate();
  emit UpdateData();
}

void SingleEntry::EditData(GeneralData const &oldEntry, GeneralData const &newEntry)
{
  m_internalData->materialTotal += (GetMaterialPrice(newEntry) - GetMaterialPrice(oldEntry));
  m_internalData->helperTotal += (GetHelperPrice(newEntry) - GetHelperPrice(oldEntry));
  m_internalData->serviceTotal += (GetServicePrice(newEntry) - GetServicePrice(oldEntry));
  Calculate();
  emit UpdateData();
}

void SingleEntry::RemoveData(GeneralData const &entry)
{
  m_internalData->materialTotal -= GetMaterialPrice(entry);
  m_internalData->helperTotal -= GetHelperPrice(entry);
  m_internalData->serviceTotal -= GetServicePrice(entry);
  Calculate();
  emit UpdateData();
}

std::unique_ptr<Data> SingleEntry::GetData(std::string const &id)
{
  std::unique_ptr<GeneralData> data(new GeneralData());
  if (!m_query.prepare("SELECT * FROM " + QString::fromStdString(m_data.tableName) + " WHERE POSIT = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(id));
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  data->pos = m_query.value(1).toString();
  data->artNr = m_query.value(2).toString();
  data->text = m_query.value(3).toString();
  data->number = m_query.value(4).toUInt();
  data->ep = m_query.value(5).toDouble();
  data->total = m_query.value(6).toDouble();
  data->unit = m_query.value(7).toString();
  data->helpMat = m_query.value(8).toDouble();
  data->time = m_query.value(9).toDouble();
  data->discount = m_query.value(10).toDouble();
  data->ekp = m_query.value(11).toDouble();
  data->surcharge = m_query.value(12).toDouble();
  data->hourlyRate = m_query.value(13).toDouble();
  return data;
}

void SingleEntry::ImportData()
{
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Import";
  ImportWidget *import = new ImportWidget(this);
  connect(import, &ImportWidget::Close, [this, tabName]()
  {
    CloseTab(tabName);
    setFocus();
  });
  
  import->hide();
  AddSubtab(import, tabName);
  if (import->exec() == QDialog::Accepted)
  {
    if (import->chosenTab != TabName::UndefTab && import->chosenId.size() != 0)
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
        data.helpMat = srcQuery.value(9).toDouble() / static_cast<double>(data.number);
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
    }
    ShowDatabase();
  }
  CloseTab(tabName);
}

void SingleEntry::EditMeta()
{
  throw std::runtime_error("Not implemented EditMeta for inherited class");
}

void SingleEntry::EditAfterImport(ImportWidget *import)
{
  auto input = Overwatch::GetInstance().GetTabPointer(import->chosenTab)->GetData(import->chosenId);
  std::unique_ptr<GeneralMainData> data(static_cast<GeneralMainData*>(input.release()));
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

void SingleEntry::OnEscape()
{
  m_db = QSqlDatabase();
  m_db.removeDatabase("general_" + m_data.tabName);
  emit CloseTab(m_data.tabName + ":" + QString::number(m_number));
}
