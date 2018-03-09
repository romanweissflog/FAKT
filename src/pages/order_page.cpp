#include "pages/order_page.h"

#include "ui_order_content.h"
#include "ui_page_framework.h"

OrderContent::OrderContent(QSqlQuery &query, QString const &table, QWidget *parent)
  : m_ui(new Ui::orderContent)
{
  m_ui->setupUi(this);

  m_ui->tableData->setColumnCount(3);
  m_ui->tableData->setHorizontalHeaderLabels({ "Position", "Artikelnummer", "Bezeichnung" });
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

  m_ui->tableData->horizontalHeader()->setSectionsMovable(true);
  m_ui->tableData->horizontalHeader()->setDragEnabled(true);
  m_ui->tableData->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
  m_ui->tableData->verticalHeader()->setSectionsMovable(true);
  m_ui->tableData->verticalHeader()->setDragEnabled(true);
  m_ui->tableData->verticalHeader()->setDragDropMode(QAbstractItemView::InternalMove);

  QString sql = "SELECT POSIT, ARTNR, HAUPTARTBEZ FROM " + table;
  auto rc = query.exec();
  if (rc)
  {
    int count{};
    while (query.next())
    {
      m_ui->tableData->insertRow(count);
      QString const pos = query.value(0).toString();
      m_ui->tableData->setItem(count, 0, new QTableWidgetItem(pos));
      m_ui->tableData->setItem(count, 1, new QTableWidgetItem(query.value(1).toString()));
      m_ui->tableData->setItem(count, 2, new QTableWidgetItem(query.value(2).toString()));
      mapping.emplace(pos, pos);
      ++count;
    }
  }
}

void OrderContent::ReOrder()
{

}


OrderPage::OrderPage(QSqlQuery &query, QString const &table, QWidget *parent)
  : PageFramework(parent)
  , content(new OrderContent(query, table, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
}

OrderPage::~OrderPage()
{}
