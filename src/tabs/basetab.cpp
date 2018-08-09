#include "tabs\basetab.h"
#include "functionality\log.h"
#include "functionality\overwatch.h"
#include "functionality\sql_helper.hpp"
#include "functionality\position.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtPrintSupport\qprintdialog.h"
#include "QtWidgets\qmessagebox.h"
#include "QtGui\qevent.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace
{
  bool CompareIds(QString const &s1, QString const &s2)
  {
    auto isNumber = [](const std::string& s)
    {
      return s.size() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
    };

    auto const strS1 = s1.toStdString();
    auto const strS2 = s2.toStdString();
    if (isNumber(strS1) && isNumber(strS2))
    {
      auto const intS1 = std::stoll(strS1);
      auto const intS2 = std::stoll(strS2);
      if (intS1 > 900000 && intS2 < 900000)
      {
        return true;
      }
      else if (intS1 < 900000 && intS2 > 900000)
      {
        return false;
      }
      return std::stoll(strS1) < std::stoll(strS2);
    }
    return s1 < s2;
  }
}

BaseTab::BaseTab(TabData const &childData, QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new CustomSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
  , m_export(childData.printMask)
  , m_logId(Log::GetLog().RegisterInstance(childData.type))
  , m_data(childData)
  , m_currentRow(-1)
{
  m_ui->setupUi(this);
  m_ui->newEntry->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->editEntry->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->deleteEntry->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->search->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->filter->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->pdfExport->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->printEntry->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->goBack->installEventFilter(Overwatch::GetInstance().GetEventLogger());

  m_ui->databaseView->setModel(m_proxyModel);
  m_ui->databaseView->setSortingEnabled(true);
  m_ui->databaseView->horizontalHeader()->setSectionsMovable(true);
  m_ui->databaseView->horizontalHeader()->setDragEnabled(true);
  m_ui->databaseView->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
  m_ui->databaseView->verticalHeader()->setSectionsMovable(true);
  m_ui->databaseView->verticalHeader()->setDragEnabled(true);
  m_ui->databaseView->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);

  m_proxyModel->setSourceModel(m_model);
  m_proxyModel->setFilterKeyColumn(-1);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);

  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::EditEntryAfterClick);
  connect(&m_export, &Export::Created, [this](QWidget *page)
  {
    emit AddSubtab(page, m_data.tabName + ":Export");
  });
  connect(&m_export, &Export::Close, [this]()
  {
    emit CloseTab(m_data.tabName + ":Export");
    setFocus();
  });
  connect(m_ui->goBack, &QPushButton::clicked, this, &BaseTab::OnEscape);
  m_ui->goBack->installEventFilter(Overwatch::GetInstance().GetEventLogger());

  for (auto &&e : m_data.columns)
  {
    if (std::find(std::begin(m_data.defaultSelection), std::end(m_data.defaultSelection), e.first) != std::end(m_data.defaultSelection))
    {
      m_tableFilter[e.first] = true;
    }
    else
    {
      m_tableFilter[e.first] = false;
    }
  }

  SHORTCUTMAP(Key_N, AddEntry)
  SHORTCUTMAP(Key_B, EditEntry)
  SHORTCUTMAP(Key_L, DeleteEntry)
  SHORTCUTMAP(Key_U, SearchEntry)
  SHORTCUTMAP(Key_A, FilterList)
  SHORTCUTMAP(Key_P, ExportToPDF)
  SHORTCUTMAP(Key_D, PrintEntry)
  SHORTCUTMAP(Key_Escape, OnEscape)
}

BaseTab::~BaseTab()
{
}

void BaseTab::SetSettings(Settings *settings)
{
  m_settings = settings;
}

void BaseTab::SetDatabase(QSqlDatabase &db)
{
  m_query = QSqlQuery(db);
  QString sql = "SELECT " + m_data.idString + " FROM " + QString::fromStdString(m_data.tableName);
  m_rc = m_query.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogTypeError, m_logId, m_query.lastError().text().toStdString());
  }
  m_ids.clear();
  while (m_query.next())
  {
    m_ids.push_back(m_query.value(0).toString());
  }
  ShowDatabase();
}

void BaseTab::ShowDatabase()
{
  std::string sql = "SELECT ";
  for (auto &&s : m_data.columns)
  {
    if (m_tableFilter[s.first])
    {
      sql += s.first + ", ";
    }
  }
  sql = sql.substr(0, sql.size() - 2);
  sql += " FROM " + m_data.tableName;
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

  m_model->setQuery(m_query);
  size_t idx = 0;
  for (auto &&s : m_data.columns)
  {
    if (m_tableFilter[s.first])
    {
      m_ui->databaseView->horizontalHeader()->setSectionResizeMode((int)idx, s.first == "HAUPTARTBEZ" ? QHeaderView::ResizeToContents : QHeaderView::Stretch);
      m_model->setHeaderData((int)idx, Qt::Horizontal, s.second);
      idx++;
    }
  }
  while (m_model->canFetchMore())
  {
    m_model->fetchMore();
  }

  if (m_currentRow < 0)
  {
    m_ui->databaseView->scrollToBottom();
    if (m_model->rowCount() > m_settings->constants.rowOffset)
    {
      m_ui->databaseView->selectRow(m_model->rowCount() - m_settings->constants.rowOffset);
    }
  }
  else
  {
    m_ui->databaseView->selectRow(std::min(m_currentRow, m_model->rowCount() - m_settings->constants.rowOffset));
  }
  m_ui->databaseView->clearSelection();
}

void BaseTab::SearchEntry()
{
  SearchFilter *dialog = new SearchFilter();
  if (dialog->exec() == QDialog::Accepted)
  {
    QString entry = dialog->entry;
    m_proxyModel->setFilterFixedString(entry);
    ShowDatabase();
  }
}

void BaseTab::EditEntryAfterClick(QModelIndex const &)
{
  EditEntry();
}

void BaseTab::FilterList()
{
  std::map<std::string, QString> mapping;
  for (auto &&s : m_data.columns)
  {
    mapping[s.first] = s.second;
  }
  FilterTable *filter = new FilterTable(m_tableFilter, mapping, m_data.idString, this);
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

ReturnValue BaseTab::PrepareDoc(bool withLogo)
{
  auto const index = m_ui->databaseView->currentIndex();
  if (index.row() == -1 || index.column() == -1)
  {
    return ReturnValue::ReturnFailure;
  }

  QString const id = m_ui->databaseView->model()->data(index.model()->index(index.row(), 0)).toString();
  QString const tableId = util::GetPaddedNumber(id);
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside PrepareDoc with number " + id.toStdString());

  QSqlQuery extraQuery = PrepareExtraQuery(QString::fromStdString(m_data.tableName), id.toStdString());

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", m_data.tabName);
  dataDb.setDatabaseName(m_data.dataDatabase);
  dataDb.open();

  QSqlQuery groupQuery = PrepareGroupQuery("SELECT POSIT, HAUPTARTBEZ, GP FROM " + m_data.dataTableSuffix + tableId, dataDb);

  QSqlQuery dataQuery = PreparePositionsQuery(m_data.dataTableSuffix + tableId, dataDb);
  
  ReturnValue rv = m_export(m_data.tabType,
    m_query,
    dataQuery,
    groupQuery,
    extraQuery,
    withLogo ? printmask::Pdf : printmask::Print);

  dataDb = QSqlDatabase();
  dataDb.removeDatabase(m_data.tabName);

  return rv;
}

QSqlQuery BaseTab::PrepareGroupQuery(QString const &sql, QSqlDatabase const &db)
{
  QSqlQuery inputQuery(db);
  m_rc = inputQuery.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, inputQuery.lastError().text().toStdString());
    return QSqlQuery("");
  }
  auto const data = util::GetPartialSums(inputQuery);
  
  QSqlQuery query(*Overwatch::GetInstance().GetDatabase());
  m_rc = query.exec("DELETE FROM GROUPS");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  for (auto &&d : data)
  {
    QString inputSql = "INSERT INTO GROUPS (POS, ARTBEZ, BRUTTO) VALUES ('";
    inputSql += QString::number(d.first) + "', '" + d.second.first + "', '"
      + QString::number(d.second.second, 'f', 2) + "')";

    m_rc = query.exec(inputSql);
    if (!m_rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
      return QSqlQuery("");
    }
  }

  m_rc = query.exec("SELECT * from GROUPS");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  return query;
}

QSqlQuery BaseTab::PrepareExtraQuery(QString const &type, std::string const &number)
{
  QSqlQuery query(*Overwatch::GetInstance().GetDatabase());
  m_rc = query.exec("DELETE FROM PRINT_DATA");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  // check if LIEFDAT is contained
  m_rc = m_query.exec("PRAGMA TABLE_INFO(" + type + ")");
  bool foundDeliveryDate{};
  while (m_query.next())
  {
    if (m_query.value(1).toString().toStdString() == "LIEFDAT")
    {
      foundDeliveryDate = true;
      break;
    }
  }

  auto input = GetData(number);
  std::unique_ptr<GeneralMainData> data(static_cast<GeneralMainData*>(input.release()));

  std::string discountText = std::abs(data->discount) < std::numeric_limits<double>::epsilon() ? "" : m_settings->discountText.toStdString();
  double discountValue = util::Precision2Round((100.0 - data->discount) / 100.0 * data->brutto);

  // bindvalue doesnt work....workaround
  auto replace = [this](std::string &text, std::string const &placeHolder, double replacement, int precicion)
  {
    auto pos = text.find(placeHolder);
    if (pos == std::string::npos)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, "Could not replace inside sql string");
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precicion) << replacement;
    text.replace(pos, 3, stream.str());
  };

  auto setRabattBindings = [&]()
  {
    if (discountText.size() == 0)
    {
      return;
    }
    replace(discountText, ":RP", data->discount, 2);
    replace(discountText, ":RB", discountValue, 2);
  };
  setRabattBindings();

  std::string skontoText = "";
  if (!(std::abs(data->payNormal) < std::numeric_limits<double>::epsilon()))
  {
    if (std::abs(data->skonto) < std::numeric_limits<double>::epsilon())
    {
      skontoText = m_settings->skontoTextShort.toStdString();
      replace(skontoText, ":PN", data->payNormal, 0);
    }
    else
    {
      double skontoPayment = util::Precision2Round((100.0 - data->skonto) / 100.0 * discountValue);
      skontoText = m_settings->skontoTextLong.toStdString();
      replace(skontoText, ":PS", data->paySkonto, 0);
      replace(skontoText, ":SP", data->skonto, 2);
      replace(skontoText, ":SB", skontoPayment, 2);
      replace(skontoText, ":PN", data->payNormal, 0);
    }
  }

  std::string headlineText = data->headline.toStdString();
  if (foundDeliveryDate)
  {
    std::unique_ptr<InvoiceData> invoiceData(static_cast<InvoiceData*>(data.release()));
    headlineText.insert(0, "Liefer-/Leistungszeitraum: " + invoiceData->deliveryDate.toStdString() + "\n\n");
  }

  std::string sql = GenerateInsertCommand("PRINT_DATA"
    , SqlPair("TYP", type)
    , SqlPair("SKONTO", skontoText)
    , SqlPair("RABATT", discountText)
    , SqlPair("HEADLIN", headlineText));
  if (!query.prepare(QString::fromStdString(sql)))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  m_rc = query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  m_rc = query.exec("SELECT * FROM PRINT_DATA");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  return query;
}

QSqlQuery BaseTab::PreparePositionsQuery(QString const &table, QSqlDatabase const &db)
{
  try
  {
    QSqlQuery dataQuery(db);

    QSqlQuery mainQuery(*Overwatch::GetInstance().GetDatabase());
    m_rc = mainQuery.exec("DELETE FROM POSITION_DATA");
    if (!m_rc)
    {
      throw std::runtime_error(mainQuery.lastError().text().toStdString());
    }

    QString sql = "SELECT POSIT, HAUPTARTBEZ, ARTBEZ, MENGE, ME, EP, GP, ARTNR FROM " + table;
    m_rc = dataQuery.exec(sql);
    if (!m_rc)
    {
      throw std::runtime_error(dataQuery.lastError().text().toStdString());
    }
    std::vector<GeneralData> data;
    while (dataQuery.next())
    {
      GeneralData current;
      current.pos = dataQuery.value(0).toString();
      current.mainText = dataQuery.value(1).toString();
      current.text = dataQuery.value(2).toString();
      current.number = dataQuery.value(3).toDouble();
      current.unit = dataQuery.value(4).toString();
      current.ep = dataQuery.value(5).toDouble();
      current.total = dataQuery.value(6).toDouble();
      current.artNr = dataQuery.value(7).toString();
      data.push_back(current);
    }
    std::sort(std::begin(data), std::end(data), 
      [](GeneralData const &d1, GeneralData const &d2)
    {
      Position p1(d1.pos.toStdString());
      Position p2(d2.pos.toStdString());
      return p1 < p2;
    });

    auto checkForGroup = [](QString const &txt) -> bool
    {
      std::string str = txt.toStdString();
      return str.find("#") != std::string::npos;
    };

    for (auto &&d : data)
    {
      bool isGroup = checkForGroup(d.artNr);
      std::string stdString = GenerateInsertCommand("POSITION_DATA"
        , SqlPair("POSIT", d.pos)
        , SqlPair("HAUPTARTBEZ", d.mainText)
        , SqlPair("ARTBEZ", d.text)
        , SqlPair("MENGE", isGroup ? "" : QString::number(d.number))
        , SqlPair("ME", d.unit)
        , SqlPair("EP", isGroup ? "" : QString::number(d.ep, 'f', 2))
        , SqlPair("GP", isGroup ? "" : QString::number(d.total, 'f', 2)));
      m_rc = mainQuery.exec(QString::fromStdString(stdString));
      if (!m_rc)
      {
        throw std::runtime_error(mainQuery.lastError().text().toStdString());
      }
    }
    m_rc = mainQuery.exec("SELECT * FROM POSITION_DATA");
    if (!m_rc)
    {
      throw std::runtime_error(mainQuery.lastError().text().toStdString());
    }
    return mainQuery;
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogTypeError, m_logId, e.what());
    return QSqlQuery();
  }
}

void BaseTab::ExportToPDF()
{
  if (PrepareDoc(true) != ReturnValue::ReturnSuccess)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Could not print report to file");
  }
}

void BaseTab::PrintEntry()
{
  if (PrepareDoc(false) == ReturnValue::ReturnSuccess)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Could not print report");
  }
}

std::unique_ptr<Data> BaseTab::GetData(std::string const &artNr)
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "GetData not implemented for derived class");
  return std::unique_ptr<Data>();
}

void BaseTab::SetData(Data*)
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "SetData not implemented for derived class");
  return;
}

std::map<QString, std::vector<QString>> BaseTab::GetRowData(std::vector<QString> const &columns)
{
  std::map<QString, std::vector<QString>> list;
  QString sql = "SELECT";
  for (auto it = std::begin(columns); it != std::end(columns); ++it)
  {
    sql += " " + *it;
    if (it != std::prev(std::end(columns)))
    {
      sql += ",";
    }
  }
  sql += " FROM " + QString::fromStdString(m_data.tableName);
  m_rc = m_query.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return {};
  }
  while (m_query.next())
  {
    for (size_t i{}; i < columns.size(); ++i)
    {
      list[columns[i]].push_back(m_query.value((int)i).toString());
    }
  }
  return list;
}

void BaseTab::OnEscape()
{
  emit CloseTab(m_data.tabName);
}

void BaseTab::AddEntry(std::optional<GeneralData> const &)
{
  return;
}

void BaseTab::DeleteEntry()
{
  QItemSelectionModel *select = m_ui->databaseView->selectionModel();
  if (!select->hasSelection())
  {
    return;
  }
  QMessageBox *question = util::GetDeleteMessage(this);
  if (question->exec() == QMessageBox::Yes)
  {
    Log::GetLog().Write(LogTypeInfo, m_logId, "Inside DeleteEntry");
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
      DeleteData(k);
      RemoveLastKey(k);
      DeleteDataTable(k);
    }
    ShowDatabase();
  }
}

void BaseTab::DeleteDataTable(QString const &)
{
  return;
}

void BaseTab::DeleteData(QString const &key)
{
  Log::GetLog().Write(LogTypeInfo, m_logId, "Inside DeleteData with key " + key.toStdString());
  QString const sql = "DELETE FROM " + QString::fromStdString(m_data.tableName) + " WHERE " + m_data.idString + " = :ID";
  m_rc = m_query.prepare(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", key);
  m_rc = m_query.exec();
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
}

void BaseTab::EditEntry()
{
  return;
}

void BaseTab::AddAndSetLastKey(QString const &key)
{
  auto it = std::find(std::begin(m_ids), std::end(m_ids), key);
  if (it != std::end(m_ids))
  {
    Log::GetLog().Write(LogTypeError, m_logId, "id already exists in id list for adding");
    return;
  }
  m_ids.push_back(key);
  std::sort(std::begin(m_ids), std::end(m_ids), CompareIds);
  it = std::find(std::begin(m_ids), std::end(m_ids), key);
  if (it == std::end(m_ids))
  {
    Log::GetLog().Write(LogTypeError, m_logId, "Could not find id inside id list for adding");
    return;
  }
  m_currentRow = std::distance(std::begin(m_ids), it);
}

void BaseTab::EditLastKey(QString const &oldKey, QString const &newKey)
{
  auto it = std::find(std::begin(m_ids), std::end(m_ids), oldKey);
  if (it == std::end(m_ids))
  {
    Log::GetLog().Write(LogTypeError, m_logId, "Could not find id inside id list for editing");
    return;
  }
  m_ids.erase(it);
  m_ids.push_back(newKey);
  std::sort(std::begin(m_ids), std::end(m_ids), CompareIds);
  it = std::find(std::begin(m_ids), std::end(m_ids), newKey);
  m_currentRow = std::distance(std::begin(m_ids), it);
}

void BaseTab::RemoveLastKey(QString const &key)
{
  auto it = std::find(std::begin(m_ids), std::end(m_ids), key);
  if (it == std::end(m_ids))
  {
    Log::GetLog().Write(LogTypeError, m_logId, "Could not find id inside id list for removing");
    return;
  }
  m_ids.erase(it);
}
