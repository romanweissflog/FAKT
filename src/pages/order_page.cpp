#include "pages/order_page.h"

#include "ui_order_content.h"
#include "ui_page_framework.h"

#include "QtWidgets\qshortcut.h"

#include <algorithm>

namespace
{
  struct Position
  {
    int integralPart;
    int fractionalPart;

    Position()
      : integralPart(0)
      , fractionalPart(0)
    {}

    Position(std::string const &txt)
    {
      size_t posLeft = txt.find(".");
      if (posLeft == std::string::npos)
      {
        integralPart = std::stol(txt);
        fractionalPart = 0;
      }
      else
      {
        integralPart = std::stol(txt.substr(0, posLeft));
        fractionalPart = std::stol(txt.substr(posLeft + 1, txt.size() - posLeft - 1));
      }
    }

    Position& operator++()
    {
      fractionalPart++;
      return *this;
    }

    std::string ToString() const
    {
      if (fractionalPart == 0)
      {
        return std::to_string(integralPart);
      }
      return std::to_string(integralPart) + "." + std::to_string(fractionalPart);
    }

    friend bool operator<(Position const &left, Position const &right)
    {
      if (left.integralPart < right.integralPart)
      {
        return true;
      }
      if (left.integralPart > right.integralPart)
      {
        return false;
      }
      return left.fractionalPart < right.fractionalPart;
    }

    friend bool operator==(Position const &left, Position const &right)
    {
      return left.integralPart == right.integralPart && left.fractionalPart == right.fractionalPart;
    }
  };

  class PositionItem : public QTableWidgetItem
  {
  public:
    PositionItem(const QString &txt = {})
      :QTableWidgetItem(txt)
    {}

    bool operator <(const QTableWidgetItem &other) const
    {
      Position left(text().toStdString());
      Position right(other.text().toStdString());
      return left < right;
    }
  };

  Position operator-(Position const &lhs, Position const &rhs)
  {
    Position retValue;
    retValue.integralPart = rhs.integralPart - lhs.integralPart;
    retValue.fractionalPart = rhs.fractionalPart - lhs.fractionalPart;
    return retValue;
  }
}

OrderContent::OrderContent(QSqlQuery &query, QString const &table, QWidget *parent)
  : m_ui(new Ui::orderContent)
{
  m_ui->setupUi(this);

  m_ui->tableData->setSortingEnabled(true);
  m_ui->tableData->verticalHeader()->setVisible(false);
  m_ui->tableData->setColumnCount(3);
  m_ui->tableData->setHorizontalHeaderLabels({ "Position", "Artikelnummer", "Bezeichnung" });
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_ui->tableData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

  QString sql = "SELECT POSIT, ARTNR, HAUPTARTBEZ FROM " + table;
  auto rc = query.exec();
  if (rc)
  {
    int count{};
    while (query.next())
    {
      m_ui->tableData->insertRow(count);
      QString const pos = query.value(0).toString();
      QString const number = query.value(1).toString();
      QString const description = query.value(2).toString();
      m_ui->tableData->setItem(count, 0, new PositionItem(pos));
      m_ui->tableData->setItem(count, 1, new QTableWidgetItem(number));
      m_ui->tableData->setItem(count, 2, new QTableWidgetItem(description));
      mapping.push_back({ pos.toStdString(), { pos.toStdString(), number, description } });
      ++count;
    }
  }

  connect(m_ui->buttonReOrder, &QPushButton::clicked, this, &OrderContent::ReOrder);
  connect(new QShortcut(QKeySequence(Qt::Key_F8), this), &QShortcut::activated, this, &OrderContent::ReOrder);

  connect(m_ui->tableData, &QTableWidget::cellChanged, [this](int row, int column)
  {
    if (column != 0)
    {
      return;
    }
    mapping[row].second.position = m_ui->tableData->item(row, column)->text().toStdString();
  });
}

void OrderContent::ReOrder()
{
  for (auto &m : mapping)
  {
    auto &pos = m.second.position;
    pos.erase(std::remove(pos.begin(), pos.end(), '_'), pos.end());
  }
  std::sort(std::begin(mapping), std::end(mapping), [](std::pair<std::string, Data> const &lhs, std::pair<std::string, Data> const &rhs)
  {
    return Position(lhs.second.position) < Position(rhs.second.position);
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
    if (dif.integralPart == 0)
    {
      right = left;
      ++right;
      n->second.position = right.ToString();
    }
    else
    {
      right.integralPart = left.integralPart + 1;
      right.fractionalPart = 0;
      n->second.position = right.ToString();
    }
  }

  int count{};
  for (auto &&m : mapping)
  {
    auto &&c = m.second;
    m_ui->tableData->setItem(count, 0, new PositionItem(QString::fromStdString(c.position)));
    m_ui->tableData->setItem(count, 1, new PositionItem(c.number));
    m_ui->tableData->setItem(count, 2, new PositionItem(c.description));
    ++count;
  }
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
