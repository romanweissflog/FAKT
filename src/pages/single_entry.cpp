#include "pages\single_entry.h"
#include "pages\general_page.h"
#include "functionality\sql_helper.hpp"
#include "functionality\overwatch.h"
#include "pages\summary_page.h"
#include "pages\percentage_page.h"
#include "pages\order_page.h"
#include "functionality\position.h"

#include "ui_basetab.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qmessagebox.h"
#include "QtWidgets\qshortcut.h"
#include "QtGui\qevent.h"

#include <iostream>
#include <regex>
#include <sstream>


namespace
{
  TabData tabData
  {
    TabName::UndefTab,
    "SingleEntry",
    "",
    "",
    "",
    "",
    "POSIT",
    printmask::Undef,
    {
      { "POSIT", "Pos" },
      { "ARTNR", "Art.-Nr." },
      { "HAUPTARTBEZ", "Bezeichnung" }, 
      { "ARTBEZ", "Extra-Information" },
      { "ME", "Einheit" },
      { "MENGE", "Menge" },
      { "EP", "EP" },
      { "MP", "Material" },
      { "LP", "Leistung" },
      { "SP", "Sonder" },
      { "GP", "GP" },
      { "BAUZEIT", "Bauzeit" },
      { "P_RABATT", "Rabatt" },
      { "MULTI", "Aufschlag" },
      { "STUSATZ", "Stundensatz" },
      { "EKP", "EKP" }
    },
    { "POSIT", "ARTNR", "HAUPTARTBEZ", "MENGE", "EP", "GP" }
  };

  TabData GetTabData(size_t number, std::string const &prefix)
  {
    TabData data = tabData;
    std::stringstream ss;
    ss << "'" << prefix;
    size_t tmpNumber = number;
    while (tmpNumber < 100000)
    {
      ss << "0";
      tmpNumber *= 10;
    }
    ss << number << "'";
    data.tableName = ss.str();
    return data;
  }
}

SingleEntry::SingleEntry(size_t number,
  std::string const &prefix,
  TabName const &childType,
  QWidget *parent)
  : BaseTab(GetTabData(number, prefix), parent)
  , m_number(number)
  , m_childTab(childType)
  , m_nextKey("1")
{
  setAttribute(Qt::WA_DeleteOnClose);

  if (m_childTab == TabName::OfferTab)
  {
    m_internalData.reset(new OfferData());
    m_childType = "Angebot";
  }
  else
  {
    m_internalData.reset(new InvoiceData());
    if (m_childTab == TabName::InvoiceTab)
    {
      m_childType = "Rechnung";
    }
    else
    {
      m_childType = "Baustelle";
    }
  }

  QPushButton *insertData = new QPushButton(QString::fromStdString("Einf" + german::ue + "gen (E)"), this);
  m_ui->layoutAction->insertWidget(1, insertData);
  connect(insertData, &QPushButton::clicked, this, &SingleEntry::InsertEntry);

  QPushButton *editMeta = new QPushButton("Allgemein (G)", this);
  m_ui->layoutAction->addWidget(editMeta);
  connect(editMeta, &QPushButton::clicked, this, &SingleEntry::EditMeta);

  QPushButton *importButton = new QPushButton("Import (I)", this);
  m_ui->layoutAction->addWidget(importButton);
  connect(importButton, &QPushButton::clicked, this, &SingleEntry::ImportData);

  QPushButton *sumButton = new QPushButton("Summe (S)", this);
  m_ui->layoutAction->addWidget(sumButton);
  connect(sumButton, &QPushButton::clicked, this, &SingleEntry::SummarizeData);

  QPushButton *percentageButton = new QPushButton("Kalkulation (K)", this);
  m_ui->layoutAction->addWidget(percentageButton);
  connect(percentageButton, &QPushButton::clicked, this, &SingleEntry::CalcPercentages);

  QPushButton *orderButton = new QPushButton("Ordnen (O)", this);
  m_ui->layoutAction->addWidget(orderButton);
  connect(orderButton, &QPushButton::clicked, this, &SingleEntry::Order);

  m_internalData->number = QString::number(m_number);

  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(InsertEntry()));
  new QShortcut(QKeySequence(Qt::Key_G), this, SLOT(EditMeta()));
  new QShortcut(QKeySequence(Qt::Key_I), this, SLOT(ImportData()));
  new QShortcut(QKeySequence(Qt::Key_S), this, SLOT(SummarizeData()));
  new QShortcut(QKeySequence(Qt::Key_K), this, SLOT(CalcPercentages()));
  new QShortcut(QKeySequence(Qt::Key_O), this, SLOT(Order()));
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
    else if (h.first == "HAUPTARTBEZ")
    {
      continue;
    }
    else
    {
      sql += h.first + " TEXT, ";
    }
  }
  sql += "HAUPTARTBEZ TEXT);";
  m_rc = m_query.exec(QString::fromStdString(sql));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }

  sql = "SELECT POSIT FROM " + m_data.tableName;
  m_rc = m_query.exec(QString::fromStdString(sql));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }

  std::vector<double> positions;
  while (m_query.next())
  {
    positions.push_back(m_query.value(0).toDouble());
  }
  if (positions.size() > 0)
  {
    auto lastPosition = std::max_element(std::begin(positions), std::end(positions));
    m_nextKey = QString::number(static_cast<int32_t>(*lastPosition) + 1);
  }

  ShowDatabase();
}

void SingleEntry::AddEntry()
{
  AddEntry(m_nextKey, false);
}

void SingleEntry::InsertEntry()
{
  auto index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return;
  }
  QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  AddEntry(schl, true);
}

void SingleEntry::AddEntry(QString const &key, bool const isInserted)
{
  try
  {
    GeneralPage *page = new GeneralPage(m_settings, m_number, m_childType, key, this);
    page->setWindowTitle("Neuer Eintrag");
    QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Neu";
    connect(page, &PageFramework::AddExtraPage, [this, page, tabName](QWidget *widget, QString const &txt)
    {
      emit AddSubtab(widget, tabName + ":" + txt);
    });
    connect(page, &PageFramework::CloseExtraPage, [this, page, tabName](QString const &txt)
    {
      emit CloseTab(tabName + ":" + txt);
    });
    emit AddSubtab(page, tabName);
    connect(page, &PageFramework::Accepted, [this, page, tabName, isInserted]()
    {
      auto &entryData = page->content->data;
      if (entryData.pos.size() == 0)
      {
        QMessageBox::warning(this, tr("Hinweis"),
          tr("Position ist leer - Eintrag wird nicht gespeichert"));
      }
      else
      {
        QString const position = entryData.pos + (isInserted ? "_" : "");
        std::string sql = GenerateInsertCommand(m_data.tableName
          , SqlPair("POSIT", position)
          , SqlPair("ARTNR", entryData.artNr)
          , SqlPair("ARTBEZ", entryData.text)
          , SqlPair("ME", entryData.unit)
          , SqlPair("MENGE", entryData.number)
          , SqlPair("EP", entryData.ep)
          , SqlPair("MP", entryData.material)
          , SqlPair("LP", entryData.service)
          , SqlPair("SP", entryData.helpMat)
          , SqlPair("GP", entryData.total)
          , SqlPair("BAUZEIT", entryData.time)
          , SqlPair("P_RABATT", entryData.discount)
          , SqlPair("MULTI", entryData.surcharge)
          , SqlPair("STUSATZ", entryData.hourlyRate)
          , SqlPair("EKP", entryData.ekp)
          , SqlPair("HAUPTARTBEZ", entryData.mainText));
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
            tr("Position exisiert bereits - Eintrag wird nicht gespeichert"));
        }
        else
        {
          if (isInserted)
          {
            AdaptAfterInsert(position);
          }
          AddData(entryData);
        }
      }
      if (static_cast<int32_t>(entryData.pos.toDouble()) >= m_nextKey.toInt())
      {
        m_nextKey = QString::number(m_nextKey.toInt() + 1);
      }
      ShowDatabase();
      emit CloseTab(tabName);
    });
    connect(page, &PageFramework::Declined, [this, tabName]()
    {
      emit CloseTab(tabName);
    });
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Caught unknown runtime error " + std::string(e.what()));
    return;
  }
  catch (...)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Caught undefined exception");
    return;
  }
}

void SingleEntry::AdaptAfterInsert(QString const &key)
{
  Position position(key.toStdString().substr(0, key.size() - 1));
  QString sql = "SELECT POSIT FROM " + QString::fromStdString(m_data.tableName);
  m_rc = m_query.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<Position> keys;
  while (m_query.next())
  {
    QString const value = m_query.value(0).toString();
    if (value != key)
    {
      keys.push_back(Position(value.toStdString()));
    }
  }
  std::sort(std::begin(keys), std::end(keys));

  auto update = [this](std::string const &oldPosition, std::string const &newPosition)
  {
    auto sql = GenerateEditCommand(m_data.tableName, "POSIT", oldPosition, SqlPair("POSIT", newPosition));
    m_rc = m_query.exec(QString::fromStdString(sql));
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
  };
  if (position.fractionalPart == 0)
  {
    for (auto it = keys.rbegin(); it != keys.rend(); ++it)
    {
      if (it->integralPart < position.integralPart)
      {
        break;
      }
      auto const oldPosition = it->ToString();
      it->integralPart++;
      auto const newPosition = it->ToString();
      update(oldPosition, newPosition);
    }
  }
  else
  {
    for (auto it = keys.rbegin(); it != keys.rend(); ++it)
    {
      if (it->integralPart == position.integralPart && it->fractionalPart >= position.fractionalPart)
      {
        auto const oldPosition = it->ToString();
        it->fractionalPart++;
        auto const newPosition = it->ToString();
        update(oldPosition, newPosition);
      }
    }
  }
  auto const oldPosition = key.toStdString();
  auto const newPosition = position.ToString();
  update(oldPosition, newPosition);
}

void SingleEntry::DeleteEntry()
{
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    auto index = m_ui->databaseView->currentIndex();
    QString schl = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
    std::unique_ptr<GeneralData> entry(static_cast<GeneralData*>(GetData(schl.toStdString()).release()));
    if (!entry)
    {
      return;
    }
    DeleteData(schl);
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
  GeneralPage *page = new GeneralPage(m_settings, m_number, m_data.type, {}, this);
  page->setWindowTitle("Editiere Eintrag");
  std::unique_ptr<GeneralData> oldData(static_cast<GeneralData*>(GetData(schl.toStdString()).release()));
  if (!oldData)
  {
    return;
  }
  page->content->CopyData(oldData.get());

  QString tabName = m_data.tabName + ":" + QString::number(m_number) + ":Edit"; 
  connect(page, &PageFramework::AddExtraPage, [this, page, tabName](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, tabName + ":" + txt);
  });
  connect(page, &PageFramework::CloseExtraPage, [this, page, tabName](QString const &txt)
  {
    emit CloseTab(tabName + ":" + txt);
  });
  emit AddSubtab(page, tabName);
  connect(page, &PageFramework::Accepted, [this, page, oldData = oldData.get(), schl, tabName]()
  {
    auto &entryData = page->content->data;
    std::string sql = GenerateEditCommand(m_data.tableName, m_data.idString.toStdString(), schl.toStdString()
      , SqlPair("POSIT", entryData.pos)
      , SqlPair("ARTNR", entryData.artNr)
      , SqlPair("ARTBEZ", entryData.text)
      , SqlPair("ME", entryData.unit)
      , SqlPair("MENGE", entryData.number)
      , SqlPair("EP", entryData.ep)
      , SqlPair("MP", entryData.material)
      , SqlPair("LP", entryData.service)
      , SqlPair("SP", entryData.helpMat)
      , SqlPair("GP", entryData.total)
      , SqlPair("BAUZEIT", entryData.time)
      , SqlPair("P_RABATT", entryData.discount)
      , SqlPair("MULTI", entryData.surcharge)
      , SqlPair("STUSATZ", entryData.hourlyRate)
      , SqlPair("EKP", entryData.ekp)
      , SqlPair("HAUPTARTBEZ", entryData.mainText));
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
    EditData(*oldData, entryData);
    ShowDatabase();
    emit CloseTab(tabName);
  });
  connect(page, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
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

  m_internalData->customerNumber = data->customerNumber;
  m_internalData->date = data->date;
  m_internalData->endline = data->endline;
  m_internalData->headline = data->headline;
  m_internalData->hourlyRate = data->hourlyRate;
  m_internalData->name = data->name;
  m_internalData->number = data->number;
  m_internalData->payNormal = data->payNormal;
  m_internalData->paySkonto = data->paySkonto;
  m_internalData->place = data->place;
  m_internalData->salutation = data->salutation;
  m_internalData->discount = data->discount;
  m_internalData->skonto = data->skonto;
  m_internalData->street = data->street;
  m_internalData->subject = data->subject;
}

void SingleEntry::AddData(GeneralData const &entry)
{
  m_internalData->materialTotal += entry.number * entry.material;
  m_internalData->helperTotal += entry.number * entry.helpMat;
  m_internalData->serviceTotal += entry.number * entry.service;
  Calculate();
  emit UpdateData();
}

void SingleEntry::EditData(GeneralData const &oldEntry, GeneralData const &newEntry)
{
  m_internalData->materialTotal += (newEntry.number * newEntry.material - oldEntry.number * oldEntry.material);
  m_internalData->helperTotal += (newEntry.number * newEntry.helpMat - oldEntry.number * oldEntry.helpMat);
  m_internalData->serviceTotal += (newEntry.number * newEntry.service - oldEntry.number * oldEntry.service);
  Calculate();
  emit UpdateData();
}

void SingleEntry::RemoveData(GeneralData const &entry)
{
  m_internalData->materialTotal -= entry.number * entry.material;
  m_internalData->helperTotal -= entry.number * entry.helpMat;
  m_internalData->serviceTotal -= entry.number * entry.service;
  Calculate();
  emit UpdateData();
}

void SingleEntry::Recalculate(Data *edited)
{
  GeneralMainData *data = reinterpret_cast<GeneralMainData*>(edited);
  data->brutto = m_internalData->brutto;
  data->helperTotal = m_internalData->helperTotal;
  data->materialTotal = m_internalData->materialTotal;
  data->mwstTotal = m_internalData->mwstTotal;
  data->serviceTotal = m_internalData->serviceTotal;
  data->total = m_internalData->total;
}

std::unique_ptr<Data> SingleEntry::GetData(std::string const &id)
{
  std::unique_ptr<GeneralData> data(new GeneralData());
  if (!m_query.prepare("SELECT * FROM " + QString::fromStdString(m_data.tableName) + " WHERE POSIT = :ID"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_query.bindValue(":ID", QString::fromStdString(id));
  if (!m_query.exec())
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return std::unique_ptr<Data>();
  }
  m_query.next();
  data->pos = m_query.value(1).toString();
  data->artNr = m_query.value(2).toString();
  data->text = m_query.value(3).toString();
  data->unit = m_query.value(4).toString();
  data->number = m_query.value(5).toDouble();
  data->ep = m_query.value(6).toDouble();
  data->material = m_query.value(7).toDouble();
  data->service = m_query.value(8).toDouble();
  data->helpMat = m_query.value(9).toDouble();
  data->total = m_query.value(10).toDouble();
  data->time = m_query.value(11).toDouble();
  data->discount = m_query.value(12).toDouble();
  data->surcharge = m_query.value(13).toDouble();
  data->hourlyRate = m_query.value(14).toDouble();
  data->ekp = m_query.value(15).toDouble();
  data->mainText = m_query.value(16).toString();
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
        Log::GetLog().Write(LogType::LogTypeError, m_logId, "Invalid tab for opening corresponding database: " + m_data.tabName.toStdString());
        return;
      }
      srcDb.open();

      auto srcQuery = QSqlQuery(srcDb);
      std::string sql = "SELECT * FROM " + import->chosenId.toStdString();
      m_rc = srcQuery.exec(QString::fromStdString(sql));
      if (!m_rc)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
        return;
      }
      std::vector<GeneralData> copyValues;
      while (srcQuery.next())
      {
        GeneralData data;
        data.pos = srcQuery.value(1).toString();
        data.artNr = srcQuery.value(2).toString();
        data.text = srcQuery.value(3).toString();
        data.unit = srcQuery.value(4).toString();
        data.number = srcQuery.value(5).toDouble();
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
        data.mainText = srcQuery.value(16).toString();
        
        copyValues.emplace_back(data);
      }
      srcDb.close();
      srcDb = QSqlDatabase::database();
      srcDb.removeDatabase("general");

      for (auto &data : copyValues)
      {
        uint8_t count{};
        while (true)
        {
          data.pos += "_";
          std::string sql = GenerateInsertCommand(m_data.tableName
            , SqlPair("POSIT", data.pos)
            , SqlPair("ARTNR", data.artNr)
            , SqlPair("ARTBEZ", data.text)
            , SqlPair("ME", data.unit)
            , SqlPair("MENGE", data.number)
            , SqlPair("EP", data.ep)
            , SqlPair("MP", data.material)
            , SqlPair("LP", data.service)
            , SqlPair("SP", data.helpMat)
            , SqlPair("GP", data.total)
            , SqlPair("BAUZEIT", data.time)
            , SqlPair("P_RABATT", data.discount)
            , SqlPair("MULTI", data.surcharge)
            , SqlPair("STUSATZ", data.hourlyRate)
            , SqlPair("EKP", data.ekp)
            , SqlPair("HAUPTARTBEZ", data.mainText));
          m_rc = m_query.exec(QString::fromStdString(sql));
          if (m_rc)
          {
            break;
          }
          ++count;
          if (count == 5)
          {
            Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
            return;
          }
        }
        AddData(data);
      }
    }
    ShowDatabase();
  }
  CloseTab(tabName);
}

void SingleEntry::SummarizeData()
{
  QString table = QString::fromStdString(m_data.tableName.substr(1, m_data.tableName.size() - 2));
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Summe";
  SummaryPage *sum = new SummaryPage(*m_internalData, m_query, table, this);
  connect(sum, &SummaryPage::Close, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
  connect(sum, &SummaryPage::AddPartialSums, [this, sum, tabName]()
  {
    emit AddSubtab(sum->partialSums, tabName + ":Teilsummen");
  });
  connect(sum, &SummaryPage::ClosePartialSums, [this, tabName]()
  {
    emit CloseTab(tabName + ":Teilsummen");
  });
  emit AddSubtab(sum, tabName);
}

void SingleEntry::CalcPercentages()
{
  QString table = QString::fromStdString(m_data.tableName.substr(1, m_data.tableName.size() - 2));
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Prozente";
  PercentagePage *page = new PercentagePage(m_settings, m_data.tabName, *m_internalData, this);
  connect(page, &PercentagePage::Accepted, [this, page, table, tabName]()
  {
    auto &data = page->content->data;
    m_internalData->brutto = data.brutto;
    m_internalData->total = data.total;
    m_internalData->mwstTotal = data.mwstTotal;
    m_internalData->materialTotal = data.materialTotal;
    m_internalData->serviceTotal = data.serviceTotal;
    Overwatch::GetInstance().GetTabPointer(m_childTab)->SetData(m_internalData.get());

    std::vector<QString> pos;
    QString sql = "SELECT POSIT FROM " + table;
    m_rc = m_query.exec(sql);
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      emit CloseTab(tabName);
      return;
    }

    while (m_query.next())
    {
      pos.push_back(m_query.value(0).toString());
    }

    double percMat = page->content->percentageMaterial;
    double servMat = page->content->percentageService;
    for (auto &&p : pos)
    {
      sql = "SELECT MP, LP, STUSATZ, SP, MENGE FROM " + table + " WHERE POSIT = '" + p + "'";
      m_rc = m_query.exec(sql);
      if (!m_rc)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
        emit CloseTab(tabName);
        return;
      }
      m_query.next();
      double material = m_query.value(0).toDouble() * (100.0 + percMat) / 100.0;
      double service = m_query.value(1).toDouble() * (100.0 + servMat) / 100.0;
      double time = service / m_query.value(2).toDouble() * 60.0;
      double sp = m_query.value(3).toDouble();
      double ep = material + service + sp;
      double count = m_query.value(4).toDouble();
      double gp = count * ep;

      auto insert = GenerateEditCommand(table.toStdString(), "POSIT", p.toStdString(),
        SqlPair("EP", ep),
        SqlPair("MULTI", percMat),
        SqlPair("MP", material),
        SqlPair("LP", service),
        SqlPair("BAUZEIT", time),
        SqlPair("GP", gp));

      m_rc = m_query.exec(QString::fromStdString(insert));
      if (!m_rc)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
        emit CloseTab(tabName);
        return;
      }
    }
    ShowDatabase();
    emit CloseTab(tabName);
  });
  connect(page, &PercentagePage::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
  emit AddSubtab(page, tabName);
}

void SingleEntry::Order()
{
  OrderPage *page = new OrderPage(m_query, QString::fromStdString(m_data.tableName), this);
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Neuordnung";
  AddSubtab(page, tabName);
  connect(page, &PageFramework::Accepted, [this, page, tabName]()
  {
    auto &&mapping = page->content->mapping;
    std::string sql;
    for (auto &&m : mapping)
    {
      auto pos = m.first;
      sql = GenerateEditCommand(m_data.tableName, "POSIT", pos, SqlPair("POSIT", "_" + pos));
      m_rc = m_query.exec(QString::fromStdString(sql));
      if (!m_rc)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
        return;
      }
    }
    for (auto &&m : mapping)
    {
      auto pos = "_" + m.first;
      sql = GenerateEditCommand(m_data.tableName, "POSIT", pos, SqlPair("POSIT", m.second.position));
      m_rc = m_query.exec(QString::fromStdString(sql));
      if (!m_rc)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
        return;
      }
    }
    ShowDatabase();
    emit CloseTab(tabName);
  });
  connect(page, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}

void SingleEntry::EditMeta()
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "EditMeta not implemented for inherited class");
  return;
}

void SingleEntry::EditAfterImport(ImportWidget *import)
{
  std::regex reg("\\d+"); 
  std::smatch match;
  std::string id = import->chosenId.toStdString();
  if (!std::regex_search(id, match, reg))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Invalid chosen id for editing meta data: " + import->chosenId.toStdString());
    return;
  }
  auto tab = Overwatch::GetInstance().GetTabPointer(import->chosenTab);
  auto input = tab->GetData(match[0]);
  if (!input)
  {
    return;
  }
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
  Overwatch::GetInstance().GetTabPointer(m_childTab)->SetData(m_internalData.get());
}

void SingleEntry::OnEscape()
{
  m_db = QSqlDatabase();
  m_db.removeDatabase("general_" + m_data.tabName);
  emit CloseTab(m_data.tabName + ":" + QString::number(m_number));
}
