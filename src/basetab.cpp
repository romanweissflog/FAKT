#include "basetab.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"

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


BaseTab::BaseTab(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
  , m_proxyModel(new QSortFilterProxyModel(this))
  , m_model(new QSqlQueryModel(this))
{
  m_ui->setupUi(this);
  m_ui->databaseView->setModel(m_proxyModel);
  m_ui->databaseView->setSortingEnabled(true);
  m_proxyModel->setSourceModel(m_model);
  m_proxyModel->setFilterKeyColumn(-1);
  connect(m_ui->databaseView, &QTableView::doubleClicked, this, &BaseTab::ShowEntry);
}

BaseTab::~BaseTab()
{
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