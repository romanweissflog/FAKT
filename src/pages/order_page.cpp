#include "pages/order_page.h"
#include "functionality\position.h"

#include "ui_order_content.h"
#include "ui_page_framework.h"

#include "QtWidgets\qshortcut.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qmessagebox.h"

#include <algorithm>

namespace
{
  class PositionItem : public QTableWidgetItem
  {
  public:
    PositionItem(const QString &txt = {})
      : QTableWidgetItem(txt)
    {}

    bool operator <(const QTableWidgetItem &other) const
    {
      Position left(text().toStdString());
      Position right(other.text().toStdString());
      return left < right;
    }
  };
}

OrderContent::OrderContent(QSqlQuery &query, QString const &table, QWidget *parent)
  : m_ui(new Ui::orderContent)
{
  m_ui->setupUi(this);

  m_ui->tableData->setSortingEnabled(true);
  m_ui->tableData->verticalHeader()->setVisible(false);
  m_ui->tableData->setColumnCount(4);
  m_ui->tableData->setHorizontalHeaderLabels({ "ID", "Position", "Artikelnummer", "Bezeichnung" });
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

  QString sql = "SELECT id, POSIT, ARTNR, HAUPTARTBEZ FROM " + table;
  auto rc = query.exec(sql);
  if (rc)
  {
    int count{};
    while (query.next())
    {
      m_ui->tableData->insertRow(count);
      int const id = query.value(0).toInt();
      QString const pos = query.value(1).toString();
      QString const number = query.value(2).toString();
      QString const description = query.value(3).toString();
      QTableWidgetItem *idItem = new PositionItem(QString::number(id));
      idItem->setFlags(Qt::ItemFlag::NoItemFlags);
      m_ui->tableData->setItem(count, 0, idItem);
      m_ui->tableData->setItem(count, 1, new PositionItem(pos));
      m_ui->tableData->setItem(count, 2, new QTableWidgetItem(number));
      m_ui->tableData->setItem(count, 3, new QTableWidgetItem(description));
      mapping.push_back({ pos.toStdString(), { id, pos.toStdString(), number, description } });
      ++count;
    }
  }
  m_ui->tableData->sortByColumn(1, Qt::SortOrder::AscendingOrder);

  connect(m_ui->buttonReOrderPositions, &QPushButton::clicked, this, &OrderContent::ReOrderPositions);
  connect(new QShortcut(QKeySequence(Qt::Key_F8), this), &QShortcut::activated, this, &OrderContent::ReOrderPositions);
  connect(m_ui->buttonReOrderIds, &QPushButton::clicked, this, &OrderContent::ReOrderIds);
  connect(new QShortcut(QKeySequence(Qt::Key_F9), this), &QShortcut::activated, this, &OrderContent::ReOrderIds);

  connect(m_ui->tableData, &QTableWidget::cellChanged, [this](int row, int column)
  {
    if (column != 1)
    {
      return;
    }
    auto const id = m_ui->tableData->item(row, 0)->text().toInt();
    auto it = std::find_if(std::begin(mapping), std::end(mapping), [this, id](std::pair<std::string, Data> const &d)
    {
      return d.second.id == id;
    });
    if (it == std::end(mapping))
    {
      QMessageBox::warning(this, tr("Hinweis"),
        tr("Konnte EIntrag nicht verändern - Bitte Roman kontaktieren"));
      return;
    }
    it->second.position = m_ui->tableData->item(row, column)->text().toStdString();
  });
}

void OrderContent::ReOrderPositions()
{
  std::sort(std::begin(mapping), std::end(mapping), 
    [](std::pair<std::string, OrderContent::Data> const &d1, std::pair<std::string, OrderContent::Data> const &d2)
  {
    Position p1(d1.second.position);
    Position p2(d2.second.position);
    if (p1.integralPart == 0 && p2.integralPart != 0)
    {
      return false;
    }
    else if (p1.integralPart != 0 && p2.integralPart == 0)
    {
      return true;
    }
    else if (p1.integralPart == 0 && p2.integralPart == 0)
    {
      return d1.second.id < d2.second.id;
    }
    return p1 < p2;
  });

  Position current(0, 0);
  bool hadFirstHashTag{};
  for (auto it = std::begin(mapping); it != std::end(mapping); ++it)
  {
    auto nextHashTag = std::find_if(it, std::end(mapping), [](std::pair<std::string, OrderContent::Data> const &d)
    {
      return d.second.number.toStdString().find("#") != std::string::npos;
    });
    if (nextHashTag == it)
    {
      ++current.integralPart;
      current.fractionalPart = 0;
      hadFirstHashTag = true;
    }
    else if (it == std::begin(mapping) || !hadFirstHashTag)
    {
      ++current.integralPart;
      current.fractionalPart = 0;
    }
    else
    {
      ++current;
    }
    it->second.position = current.ToString();
  }

  AdaptTable();
}

void OrderContent::ReOrderIds()
{
  for (auto &m : mapping)
  {
    auto &pos = m.second.position;
    pos.erase(std::remove(pos.begin(), pos.end(), '_'), pos.end());
  }

  std::sort(std::begin(mapping), std::end(mapping), [this](std::pair<std::string, Data> const &d1, std::pair<std::string, Data> const &d2)
  {
    return d1.second.id < d2.second.id;
  });

  for (auto c = std::begin(mapping), n = std::next(c); n != std::end(mapping); ++c, ++n)
  {
    Position left(c->second.position), right(n->second.position);
    if (c == std::begin(mapping))
    {
      left.integralPart = 1;
      left.fractionalPart = 0;
      c->second.position = left.ToString();
    }
    auto dif = right - left;
    if ((dif.integralPart == 0 && dif.fractionalPart == 1) || (dif.integralPart == 1 && dif.fractionalPart == 0))
    {
      continue;
    }
    if (dif.integralPart < 0)
    {
      right.integralPart = left.integralPart + 1;
      n->second.position = right.ToString();
      for (auto t = std::next(n); t != std::end(mapping); ++t)
      {
        Position tmp(t->second.position);
        tmp.integralPart += -dif.integralPart + 1;
        t->second.position = tmp.ToString();
      }
    }
    else if (dif.integralPart == 0)
    {
      if (dif.fractionalPart == 0)
      {
        right.integralPart = left.integralPart + 1;
      }
      else
      {
        right = left;
        ++right;
      }
      n->second.position = right.ToString();
    }
    else
    {
      right.integralPart = left.integralPart + 1;
      right.fractionalPart = 0;
      n->second.position = right.ToString();
    }
  }

  AdaptTable();
}

void OrderContent::AdaptTable()
{
  int count{};
  m_ui->tableData->blockSignals(true);
  m_ui->tableData->clear();
  int rowCount = m_ui->tableData->rowCount();
  for (int i{}; i < rowCount; ++i)
  {
    m_ui->tableData->removeRow(0);
  }
  m_ui->tableData->setHorizontalHeaderLabels({ "ID", "Position", "Artikelnummer", "Bezeichnung" });
  for (auto &&m : mapping)
  {
    m_ui->tableData->insertRow(count);
    auto &&c = m.second;
    m_ui->tableData->setItem(count, 0, new PositionItem(QString::number(c.id)));
    m_ui->tableData->setItem(count, 1, new PositionItem(QString::fromStdString(c.position)));
    m_ui->tableData->setItem(count, 2, new PositionItem(c.number));
    m_ui->tableData->setItem(count, 3, new PositionItem(c.description));
    ++count;
  }
  m_ui->tableData->blockSignals(false);
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
