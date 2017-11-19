#include "basetab.h"
#include "log.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"
#include "QtWidgets\qcheckbox.h"
#include "QtWidgets\qcombobox.h"
#include "QtPrintSupport\qprintdialog.h"
#include "QtWidgets\qmessagebox.h"
#include "QtGui\qevent.h"

#include <iostream>

SearchFilter::SearchFilter(QWidget *parent)
  : Entry(parent)
{
  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *label = new QLabel("Suchbegriff:", this);
  QLineEdit *search = new QLineEdit(this);
  connect(search, &QLineEdit::textChanged, [this](QString txt)
  {
    entry = txt;
  });
  m_widgets.push_back(label);
  m_widgets.push_back(search);
  layout->addWidget(label);
  layout->addWidget(search);

  m_layout->insertLayout(0, layout);
  this->show();
}

SearchFilter::~SearchFilter()
{}

ShowValue::ShowValue(QString value, QWidget *parent)
  : Entry(parent)
{
  QLabel *label = new QLabel(value, this);
  m_widgets.push_back(label);

  m_layout->insertWidget(0, label);
  this->show();
}

ShowValue::~ShowValue()
{}


ShowValueList::ShowValueList(std::vector<QString> const &list, QWidget *parent)
  : Entry(parent)
{
  QComboBox *box = new QComboBox(this);
  m_widgets.push_back(box);

  box->addItem("");
  for (auto &&l : list)
  {
    box->addItem(l);
  }

  connect(box, &QComboBox::currentTextChanged, [this](QString txt)
  {
    currentItem = txt;
  });

  m_layout->insertWidget(0, box);
  this->show();
}

ShowValueList::~ShowValueList()
{}


FilterTable::FilterTable(std::map<std::string, bool> &oldFilter, 
  std::map<std::string, std::string> &mapping,
  QWidget *parent)
  : Entry(parent)
{
  size_t idx = 0;
  for (auto &&f : oldFilter)
  {
    QCheckBox *checkBox = new QCheckBox(QString::fromStdString(mapping[f.first]), this);
    checkBox->setChecked(f.second);
    connect(checkBox, &QCheckBox::stateChanged, [&](int state)
    {
      state == 2 ? oldFilter[f.first] = true : oldFilter[f.first] = false;
    });
    m_layout->insertWidget((int)idx, checkBox);
    idx++;
    m_widgets.push_back(checkBox);
  }

  this->show();
}

FilterTable::~FilterTable()
{
}


BaseTab::BaseTab(std::string const &childType, PrintType const &childPrintType, QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new QSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
  , m_export(childPrintType)
  , m_pdfPrinter(QPrinter::PrinterResolution)
  , m_printer(QPrinter::PrinterResolution)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{
  m_ui->setupUi(this);
  m_ui->databaseView->setModel(m_proxyModel);
  m_ui->databaseView->setSortingEnabled(true);
  m_proxyModel->setSourceModel(m_model);
  m_proxyModel->setFilterKeyColumn(-1);
  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::ShowEntry);

  m_pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
  m_pdfPrinter.setPaperSize(QPrinter::A4);
  m_pdfPrinter.setOutputFileName("template.pdf");

  m_printer.setPaperSize(QPrinter::A4);
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

  connect(m_ui->databaseView, SIGNAL(QTableView::doubleClicked), this, SLOT(EditEntry));

  ShowDatabase();
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

void BaseTab::ShowEntry(QModelIndex const &index)
{
  QString value = m_ui->databaseView->model()->data(index).toString();

  ShowValue *entry = new ShowValue(value, this);
}

void BaseTab::EmitToPrinter(QTextDocument &doc)
{
  QPrintDialog *pdlg = new QPrintDialog(&m_printer, this);
  if (pdlg->exec())
  {
    std::cout << "HI" << std::endl;
  }
}

void BaseTab::ExportToPDF()
{}

void BaseTab::PrintEntry()
{}

Data* BaseTab::GetData(std::string const &artNr)
{
  return nullptr;
}

std::vector<QString> BaseTab::GetArtNumbers()
{
  return{};
}

void BaseTab::closeEvent(QCloseEvent *event)
{
  QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Hinweis",
    tr("Schließen (Nicht gespeicherte Änderungen gehen verloren)?\n"),
    QMessageBox::No | QMessageBox::Yes,
    QMessageBox::Yes);
  if (resBtn != QMessageBox::Yes) 
  {
    event->ignore();
  }
  else 
  {
    event->accept();
  }
}