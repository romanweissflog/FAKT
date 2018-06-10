#include "pages\single_entry.h"
#include "pages\general_page.h"
#include "functionality\sql_helper.h"
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

#include <regex>
#include <sstream>


namespace
{
  TabData tabData
  {
    TabName::SingleEntryTab,
    "SingleEntry",
    "",
    "",
    "",
    "",
    "POSIT",
    printmask::Undef,
    {{
      { "POSIT",  { "Pos" } },
      { "ARTNR", { "Art.-Nr." } },
      { "HAUPTARTBEZ", { "Bezeichnung" } },
      { "ARTBEZ", { "Extra-Information" } },
      { "ME", { "Einheit" } },
      { "MENGE", { "Menge" } },
      { "EP", { "EP" } },
      { "MP", { "Material" } },
      { "LP", { "Leistung" } },
      { "SP", { "Sonder" } },
      { "GP", { "GP" } },
      { "BAUZEIT", { "Bauzeit" } },
      { "P_RABATT", { "Rabatt" } },
      { "MULTI", { "Aufschlag" } },
      { "STUSATZ", { "Stundensatz" } },
      { "EKP", { "EKP" } }
    }},
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
    m_childType = "Angebot";
  }
  else
  {
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

  // TBD check
  //data->number = QString::number(m_number);

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
  try
  {
    m_db = QSqlDatabase::addDatabase("QSQLITE", "general_" + m_data.tabName);
    m_db.setDatabaseName(name);
    m_db.open();
    m_query = QSqlQuery(m_db);

    std::string sql = "CREATE TABLE IF NOT EXISTS " + m_data.tableName
      + " (id INTEGER PRIMARY KEY, ";
    for (auto &&h : m_data.entries.data)
    {
      if (h.first == "POSIT")
      {
        sql += h.first.toStdString() + " TEXT UNIQUE, ";
      }
      else if (h.first == "HAUPTARTBEZ")
      {
        continue;
      }
      else
      {
        sql += h.first.toStdString() + " TEXT, ";
      }
    }
    sql += "HAUPTARTBEZ TEXT);";
    m_rc = m_query.exec(QString::fromStdString(sql));
    if (!m_rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
    }

    sql = "SELECT POSIT FROM " + m_data.tableName;
    m_rc = m_query.exec(QString::fromStdString(sql));
    if (!m_rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
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
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
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
      try
      {
        auto &&entryData = page->GetData();
        auto position = entryData["POSIT"].entry.toString();
        if (position.size() == 0)
        {
          QMessageBox::warning(this, tr("Hinweis"),
            tr("Position ist leer - Eintrag wird nicht gespeichert"));
        }
        else
        {
          entryData["POSIT"].entry = position + (isInserted ? "_" : "");
          QString sql = GenerateInsertCommand(m_data.tableName, std::begin(entryData.data), std::end(entryData.data));
          m_rc = m_query.prepare(sql);
          if (!m_rc)
          {
            throw std::runtime_error(m_query.lastError().text().toStdString());
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
        if (static_cast<int32_t>(entryData["POSIT"].entry.toDouble()) >= m_nextKey.toInt())
        {
          m_nextKey = QString::number(m_nextKey.toInt() + 1);
        }
        ShowDatabase();
        emit CloseTab(tabName);
      }
      catch (std::runtime_error e)
      {
        Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
        emit CloseTab(tabName);
      }
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
    DatabaseDataEntry updateData
    {
      { "POSIT", { "", QString::fromStdString(newPosition) } }
    };
    auto sql = GenerateEditCommand(m_data.tableName, "POSIT", QString::fromStdString(oldPosition), std::begin(updateData), std::end(updateData));
    m_rc = m_query.exec(sql);
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
  QItemSelectionModel *select = m_ui->databaseView->selectionModel();
  if (!select->hasSelection())
  {
    return;
  }
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    std::vector<QString> keys;
    for (auto &&index : select->selectedIndexes())
    {
      if (index.row() == -1 || index.column() == -1)
      {
        continue;
      }
      keys.push_back(m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString());
    }
    for (auto &&k : keys)
    {
      auto const entry = GetData(k.toStdString());
      DeleteData(k);
      RemoveData(entry);
    }
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
  QString key = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  GeneralPage *page = new GeneralPage(m_settings, m_number, m_data.type, {}, this);
  page->setWindowTitle("Editiere Eintrag");
  auto const oldData = GetData(key.toStdString());
  page->content->CopyData(oldData);

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
  connect(page, &PageFramework::Accepted, [this, page, oldData, key, tabName]()
  {
    try
    {
      auto &&entryData = page->GetData();
      BaseTab::EditData(key, entryData);
      EditData(oldData, entryData);
      ShowDatabase();
    }
    catch (std::runtime_error e)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
    }
    emit CloseTab(tabName);
  });
  connect(page, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}

void SingleEntry::SetLastData(DatabaseData const &input)
{
  data = input;
}

void SingleEntry::AddData(DatabaseData const &entry)
{
  double const size = entry["MENGE"].entry.toDouble();
  
  auto adapt = [&](QString const &key)
  {
    data[key].entry =
      data[key].entry.toDouble() +
      size * entry[key].entry.toDouble();
  };

  adapt("MGESAMT");
  adapt("SGESAMT");
  adapt("LGESAMT");

  Calculate();
  emit UpdateData();
}

void SingleEntry::EditData(DatabaseData const &oldEntry, DatabaseData const &newEntry)
{
  double const oldSize = oldEntry["MENGE"].entry.toDouble();
  double const newSize = newEntry["MENGE"].entry.toDouble();

  auto adapt = [&](QString const &key)
  {
    data[key].entry =
      data[key].entry.toDouble() +
      newSize * newEntry[key].entry.toDouble() -
      oldSize * oldEntry[key].entry.toDouble();
  };

  adapt("MGESAMT");
  adapt("SGESAMT");
  adapt("LGESAMT");

  Calculate();
  emit UpdateData();
}

void SingleEntry::RemoveData(DatabaseData const &entry)
{
  double const size = entry["MENGE"].entry.toDouble();

  auto adapt = [&](QString const &key)
  {
    data[key].entry =
      data[key].entry.toDouble() -
      size * entry[key].entry.toDouble();
  };

  adapt("MGESAMT");
  adapt("SGESAMT");
  adapt("LGESAMT");

  Calculate();
  emit UpdateData();

  Calculate();
  emit UpdateData();
}

void SingleEntry::Recalculate(DatabaseData const &data)
{
  auto adapt = [&](QString const &key)
  {
    data[key].entry = data[key].entry;
  };
  adapt("SGESAMT");
  adapt("MGESAMT");
  adapt("LGESAMT");
  adapt("GESAMT");
  adapt("MWSTGESAMT");
  adapt("BRUTTO");
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
  try
  {
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
          throw std::runtime_error("Invalid tab for opening corresponding database: " + m_data.tabName.toStdString());
        }
        srcDb.open();

        auto srcQuery = QSqlQuery(srcDb); 
        std::regex reg("\\D+");
        std::smatch match;
        std::string searchData = import->chosenId.toStdString();
        if (!std::regex_search(searchData, match, reg))
        {
          throw std::runtime_error("regex missmatch for extracting letter");
        }
        QString sql = GenerateSelectAllCommand(match[0].str() + util::GetPaddedNumber(import->chosenId).toStdString(),
          std::begin(m_data.entries.data), std::end(m_data.entries.data));
        m_rc = srcQuery.exec(sql);
        if (!m_rc)
        {
          throw std::runtime_error(srcQuery.lastError().text().toStdString());
        }
        std::vector<DatabaseData> copyValues;
        while (srcQuery.next())
        {
          DatabaseData current;
          for (auto &d : current.data)
          {
            d.second.entry = srcQuery.value(d.first);
          }
          copyValues.emplace_back(std::move(current));
        }
        srcDb.close();
        srcDb = QSqlDatabase::database();
        srcDb.removeDatabase("general");

        bool isFirst = (data.GetDouble("GESAMT") < std::numeric_limits<double>::epsilon());
        for (auto &data : copyValues)
        {
          uint8_t count{};
          while (true)
          {
            if (!isFirst)
            {
              data.GetString("POSIT") += "_";
            }
            sql = GenerateInsertCommand(m_data.tableName, std::begin(data.data), std::end(data.data));
            m_rc = m_query.exec(sql);
            if (m_rc)
            {
              break;
            }
            ++count;
            if (count == 5)
            {
              throw std::runtime_error(m_query.lastError().text().toStdString());
            }
          }
          AddData(data);
        }
      }
      ShowDatabase();
    }
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
  CloseTab(tabName);
}

void SingleEntry::SummarizeData()
{
  QString table = QString::fromStdString(m_data.tableName.substr(1, m_data.tableName.size() - 2));
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Summe";
  SummaryPage *sum = new SummaryPage(data, m_query, table, this);
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
  QString const table = QString::fromStdString(m_data.tableName.substr(1, m_data.tableName.size() - 2));
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Prozente";
  PercentagePage *page = new PercentagePage(m_settings, m_data.tabName, data, this);
  connect(page, &PercentagePage::Accepted, [this, page, table, tabName]()
  {
    auto &&data = page->GetData();
    auto adapt = [&](QString const &key)
    {
      data[key].entry = data[key].entry.toDouble();
    };
    adapt("BRUTTO");
    adapt("GESAMT");
    adapt("MWSTGESAMT");
    adapt("MGESAMT");
    adapt("LGESAMT");
    Overwatch::GetInstance().GetTabPointer(m_childTab)->SetData(data);

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

    try
    {
      double percMat = page->content->percentageMaterial;
      double servMat = page->content->percentageService;
      for (auto &&p : pos)
      {
        sql = "SELECT MP, LP, STUSATZ, SP, MENGE FROM " + table + " WHERE POSIT = '" + p + "'";
        m_rc = m_query.exec(sql);
        if (!m_rc)
        {
          throw std::runtime_error(m_query.lastError().text().toStdString());
        }
        m_query.next();
        double material = m_query.value(0).toDouble() * (100.0 + percMat) / 100.0;
        double service = m_query.value(1).toDouble() * (100.0 + servMat) / 100.0;
        double time = service / m_query.value(2).toDouble() * 60.0;
        double sp = m_query.value(3).toDouble();
        double ep = material + service + sp;
        double count = m_query.value(4).toDouble();
        double gp = count * ep;

        DatabaseData editData;
        editData["EP"].entry = ep;
        editData["MULTI"].entry = percMat;
        editData["MP"].entry = material;
        editData["LP"].entry = service;
        editData["BAUZEIT"].entry = time;
        editData["GP"].entry = gp;

        sql = GenerateEditCommand(table.toStdString(), m_data.idString, p, std::begin(editData.data), std::end(editData.data));
        m_rc = m_query.exec(sql);
        if (!m_rc)
        {
          throw std::runtime_error(m_query.lastError().text().toStdString());
        }
      }
      ShowDatabase();
    }
    catch (std::runtime_error e)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
    }
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
    try
    {
      auto &&mapping = page->content->mapping;
      QString sql;
      DatabaseData entry;
      for (auto &&m : mapping)
      {
        auto pos = QString::fromStdString(m.first);
        entry["POSIT"].entry = "_" + pos;
        sql = GenerateEditCommand(m_data.tableName, m_data.idString, pos, std::begin(entry.data), std::end(entry.data));
        m_rc = m_query.exec(sql);
        if (!m_rc)
        {
          throw std::runtime_error(m_query.lastError().text().toStdString());
        }
      }
      for (auto &&m : mapping)
      {
        auto pos = QString::fromStdString(m.first);
        entry["POSIT"].entry = "_" + pos;
        sql = GenerateEditCommand(m_data.tableName, m_data.idString, pos, std::begin(entry.data), std::end(entry.data));
        m_rc = m_query.exec(sql);
        if (!m_rc)
        {
          throw std::runtime_error(m_query.lastError().text().toStdString());
        }
      }
      ShowDatabase();
    }
    catch (std::runtime_error e)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
    }
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
  auto const input = tab->GetData(match[0]);

  auto adapt = [&](QString const &key)
  {
    data[key].entry = input[key].entry;
  };

  if (import->importAddress)
  {
    adapt("KUNR");
    adapt("NAME");
    adapt("ORT");
    adapt("ANREDE");
    adapt("STRASSE");
  }
  if (import->importEndline)
  {
    adapt("SCHLUSS");
  }
  if (import->importHeadline)
  {
    adapt("HEADLIN");
  }
  if (import->importSubject)
  {
    adapt("BETREFF");
  }
  Overwatch::GetInstance().GetTabPointer(m_childTab)->SetData(data);
}

DatabaseData SingleEntry::GetInternalData() const
{
  return data;
}

void SingleEntry::OnEscape()
{
  m_db = QSqlDatabase();
  m_db.removeDatabase("general_" + m_data.tabName);
  emit CloseTab(m_data.tabName + ":" + QString::number(m_number));
}
