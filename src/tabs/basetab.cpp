#include "tabs\basetab.h"
#include "functionality\log.h"
#include "functionality\overwatch.h"
#include "functionality\sql_helper.hpp"

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

BaseTab::BaseTab(TabData const &childData, QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new CustomSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
  , m_export(childData.printMask)
  , m_logId(Log::GetLog().RegisterInstance(childData.type))
  , m_data(childData)
{
  m_ui->setupUi(this);

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

  m_shortCuts[Qt::Key_N] = new QShortcut(QKeySequence(Qt::Key_N), this, SLOT(AddEntry()));
  m_shortCuts[Qt::Key_B] = new QShortcut(QKeySequence(Qt::Key_B), this, SLOT(EditEntry()));
  m_shortCuts[Qt::Key_L] = new QShortcut(QKeySequence(Qt::Key_L), this, SLOT(DeleteEntry()));
  m_shortCuts[Qt::Key_U] = new QShortcut(QKeySequence(Qt::Key_U), this, SLOT(SearchEntry()));
  m_shortCuts[Qt::Key_A] = new QShortcut(QKeySequence(Qt::Key_A), this, SLOT(FilterList()));
  m_shortCuts[Qt::Key_P] = new QShortcut(QKeySequence(Qt::Key_P), this, SLOT(ExportToPDF()));
  m_shortCuts[Qt::Key_D] = new QShortcut(QKeySequence(Qt::Key_D), this, SLOT(PrintEntry()));
  m_shortCuts[Qt::Key_Escape] = new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(OnEscape()));
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

  m_ui->databaseView->scrollToBottom();
  if (m_model->rowCount() > m_settings->constants.rowOffset)
  {
    m_ui->databaseView->selectRow(m_model->rowCount() - m_settings->constants.rowOffset);
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
  QString const offerId = util::GetPaddedNumber(id);

  QSqlQuery extraQuery = PrepareExtraQuery(QString::fromStdString(m_data.tableName), id.toStdString());

  m_rc = m_query.prepare("SELECT * FROM " + QString::fromStdString(m_data.tableName) + " WHERE RENR = :ID");
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

  QSqlDatabase dataDb = QSqlDatabase::addDatabase("QSQLITE", m_data.tabName);
  dataDb.setDatabaseName(m_data.dataDatabase);
  dataDb.open();

  QSqlQuery groupQuery = PrepareGroupQuery("SELECT POSIT, HAUPTARTBEZ, GP FROM " + m_data.dataTableSuffix + offerId, dataDb);

  QSqlQuery dataQuery(dataDb);

  QString sql = "SELECT * FROM " + m_data.dataTableSuffix + offerId;
  m_rc = dataQuery.exec(sql);
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, dataQuery.lastError().text().toStdString());
    return ReturnValue::ReturnFailure;
  }

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
      + QString::number(d.second.second) + "')";

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
  m_rc = query.exec("DELETE FROM EXTRA_INFO");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  auto input = GetData(number);
  std::unique_ptr<GeneralMainData> data(static_cast<GeneralMainData*>(input.release()));

  std::string discountText = std::abs(data->discount) < std::numeric_limits<double>::epsilon() ? "" : m_settings->discountText.toStdString();
  double discountValue = (100.0 - data->discount) / 100.0 * data->brutto;

  // bindvalue doesnt work....workaround
  auto replace = [](std::string &text, std::string const &placeHolder, double replacement, int precicion)
  {
    auto pos = text.find(placeHolder);
    if (pos == std::string::npos)
    {
      throw std::runtime_error("Could not replace inside sql string");
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
    if (std::abs(data->paySkonto) < std::numeric_limits<double>::epsilon())
    {
      skontoText = m_settings->skontoTextShort.toStdString();
      replace(skontoText, ":PN", data->payNormal, 0);
    }
    else
    {
      double skontoPayment = (100.0 - data->skonto) / 100.0 * discountValue;
      skontoText = m_settings->skontoTextLong.toStdString();
      replace(skontoText, ":PS", data->paySkonto, 0);
      replace(skontoText, ":SP", data->skonto, 2);
      replace(skontoText, ":SB", skontoPayment, 2);
      replace(skontoText, ":PN", data->payNormal, 0);
    }
  }

  std::string sql = GenerateInsertCommand("EXTRA_INFO"
    , SqlPair("TYP", type)
    , SqlPair("SKONTO", skontoText)
    , SqlPair("RABATT", discountText));
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

  m_rc = query.exec("SELECT * FROM EXTRA_INFO");
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, query.lastError().text().toStdString());
    return QSqlQuery("");
  }

  return query;
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

void BaseTab::AddEntry()
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
    }
    ShowDatabase();
  }
}

void BaseTab::DeleteData(QString const &key)
{
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
