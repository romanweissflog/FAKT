#include "basetab.h"
#include "log.h"

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


BaseTab::BaseTab(std::string const &childType, PrintType const &childPrintType, QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new CustomSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
  , m_export(childPrintType)
  , m_pdfPrinter(QPrinter::PrinterResolution)
  , m_printer(QPrinter::PrinterResolution)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{
  m_ui->setupUi(this);

  m_ui->databaseView->setModel(m_proxyModel);
  m_ui->databaseView->setSortingEnabled(true);
  m_ui->databaseView->verticalHeader()->setVisible(false);

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
