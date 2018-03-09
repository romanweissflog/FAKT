#include "tabs\basetab.h"
#include "functionality\log.h"

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
#include "QtWidgets\qfiledialog.h"

#include <iostream>
#include <sstream>

BaseTab::BaseTab(TabData const &childData, QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new CustomSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
  , m_export(childData.printType)
  , m_pdfPrinter(QPrinter::PrinterResolution)
  , m_printer(QPrinter::PrinterResolution)
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

  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::EditEntryAfterClick);
  connect(m_ui->databaseView, &QTableView::clicked, this, &BaseTab::HandleLeftClick);
  connect(&m_export, &Export::Created, [this](QWidget *page)
  {
    emit AddSubtab(page, m_data.tabName + ":Export");
  });
  connect(&m_export, &Export::Close, [this]()
  {
    emit CloseTab(m_data.tabName + ":Export");
    setFocus();
  });

  m_pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
  m_pdfPrinter.setPaperSize(QPrinter::A4);

  QFont font("Times", 10);
  m_doc.setDefaultFont(font);
  m_printer.setPaperSize(QPrinter::A4);

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

void BaseTab::HandleLeftClick(QModelIndex const &index)
{
  std::cout << index.row() << " " << index.column() << "\n";
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

void BaseTab::EmitToPrinter(QTextDocument &doc)
{
  QPrintDialog *pdlg = new QPrintDialog(&m_printer, this);
  if (pdlg->exec() == QPrintDialog::Accepted)
  {
    return;
  }
}

ReturnValue BaseTab::PrepareDoc(bool withLogo)
{
  return ReturnValue::ReturnFailure;
}

void BaseTab::ExportToPDF()
{
  if (PrepareDoc(true) == ReturnValue::ReturnSuccess)
  {
    QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save Pdf"), "",
      tr("pdf file (*.pdf)"));
    if (fileName.size() == 0)
    {
      return;
    }

    m_pdfPrinter.setOutputFileName(fileName);
    m_doc.print(&m_pdfPrinter);
  }
}

void BaseTab::PrintEntry()
{
  if (PrepareDoc(false) == ReturnValue::ReturnSuccess)
  {
    BaseTab::EmitToPrinter(m_doc);
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

std::vector<QString> BaseTab::GetRowData(QString const &column)
{
  std::vector<QString> list;
  m_rc = m_query.exec("SELECT " + column + " FROM " + QString::fromStdString(m_data.tableName));
  if (!m_rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return {};
  }
  while (m_query.next())
  {
    list.push_back(m_query.value(0).toString());
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
