#include "basetab.h"

#include "ui_basetab.h"

#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qlayout.h"
#include "QtCore\QModelIndex"

#include <iostream>

BaseTab::BaseTab(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::basetab)
{
  m_ui->setupUi(this);
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
