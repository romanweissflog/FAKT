#include "functionality\utils.h"
#include "functionality\utils.hpp"
#include "functionality\overwatch.h"

#include "pages\parent_page.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qlineedit.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qcheckbox.h"
#include "QtWidgets\qshortcut.h"
#include "QtWidgets\qheaderview.h"

#include <regex>
#include <sstream>

namespace
{
  struct TablePosDate
  {
    uint32_t year;
    uint32_t month;
    uint32_t day;

    TablePosDate(std::string const &date, size_t pos1, size_t pos2)
    {
      day = std::stoul(date.substr(0, pos1));
      month = std::stoul(date.substr(pos1 + 1, pos2 - pos1 - 1));
      year = std::stoul(date.substr(pos2 + 1, date.size() - pos2 - 1));
    }
  };

  bool operator<(TablePosDate const &lhs, TablePosDate const &rhs)
  {
    if (lhs.year != rhs.year)
    {
      return lhs.year < rhs.year;
    }
    else if (lhs.month != rhs.month)
    {
      return lhs.month < rhs.month;
    }
    return lhs.day < rhs.day;
  }
}


namespace util
{
  QMessageBox* GetDeleteMessage(QWidget *parent)
  {
    QMessageBox *question = new QMessageBox(parent);
    std::string msg = "Wollen sie die Eintr" + german::ae + "ge entfernen?";
    question->setWindowTitle("WARNUNG");
    question->setText(QString::fromStdString(msg));
    question->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    question->setDefaultButton(QMessageBox::No);
    question->setButtonText(QMessageBox::Yes, "Ja");
    question->setButtonText(QMessageBox::No, "No");
    return question;
  }

  bool IsDateValid(QString const &txt)
  {
    std::string data = txt.toStdString();
    if (data.size() == 0)
    {
      return true;
    }
    if (data.size() != 10)
    {
      return false;
    }
    if (data.find_first_of(".") != 2)
    {
      return false;
    }
    if (data.find_last_of(".") != 5)
    {
      return false;
    }
    return true;
  }

  bool IsNumberValid(QString const &txt)
  {
    if (txt.size() == 1 && txt.at(0) == '0')
    {
      return true;
    }
    size_t number = txt.toULongLong();
    return number != 0;
  }

  QString GetPaddedNumber(QString const &input)
  {
    std::regex reg("\\d+");
    std::smatch match;
    std::string data = input.toStdString();
    if (!std::regex_search(data, match, reg))
    {
      return input;
    }
    size_t const number = std::stoull(match[0]);
    size_t tmpNumber = number;
    std::stringstream ss;
    while (tmpNumber < 100000)
    {
      ss << "0";
      tmpNumber *= 10;
    }
    ss << number;
    return QString::fromStdString(ss.str());
  }

  PartialSumData GetPartialSums(QSqlQuery &query)
  {
    PartialSumData data;
    while (query.next())
    {
      auto const pos = query.value(0).toString().toStdString();
      auto const posPlace = pos.find(".");
      if (posPlace == std::string::npos)
      {
        auto const group = std::stoull(pos);
        data[group].first = query.value(1).toString();
        data[group].second += query.value(2).toDouble();
      }
      else if (posPlace == pos.size() - 1)
      {
        auto const group = std::stoull(pos.substr(0, posPlace));
        data[group].first = query.value(1).toString();
        data[group].second += query.value(2).toDouble();
      }
      else
      {
        auto const group = std::stoull(pos.substr(0, pos.find(".")));
        data[group].second += query.value(2).toDouble();
      }
    }
    return data;
  }
}


Entry::Entry(QWidget *parent)
  : QDialog(parent)
  , m_buttonBox(new QDialogButtonBox(this))
  , m_layout(new QVBoxLayout())
{
  QPushButton *cancelButton = new QPushButton("Cancel", this);
  m_buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  QPushButton *okButton = new QPushButton("OK", this);
  m_buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
  connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

  m_layout->addWidget(m_buttonBox);
  this->setLayout(m_layout);
}

Entry::~Entry()
{
}


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
  layout->addWidget(label);
  layout->addWidget(search);

  m_layout->insertLayout(0, layout);

  setFocus();
  search->setFocus();
  this->show();
}

SearchFilter::~SearchFilter()
{}


ShowValueList::ShowValueList(std::vector<QString> const &list, QWidget *parent)
  : Entry(parent)
{
  QComboBox *box = new QComboBox(this);

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
  std::map<std::string, QString> &mapping,
  QString const &key,
  QWidget *parent)
  : Entry(parent)
{
  size_t idx = 0;
  for (auto &&f : oldFilter)
  {
    QCheckBox *checkBox = new QCheckBox(mapping[f.first], this);
    checkBox->setChecked(f.second);
    if (f.first == key.toStdString())
    {
      checkBox->setEnabled(false);
    }
    connect(checkBox, &QCheckBox::stateChanged, [&](int state)
    {
      state == 2 ? oldFilter[f.first] = true : oldFilter[f.first] = false;
    });
    m_layout->insertWidget((int)idx, checkBox);
    idx++;
  }

  this->show();
}

FilterTable::~FilterTable()
{
}


ImportWidget::ImportWidget(QWidget *parent)
  : Entry(parent)
  , m_category(new QComboBox(this))
  , m_data(new QTableWidget(this))
  , chosenTab(TabName::UndefTab)
  , chosenId("")
  , importAddress(false)
  , importHeadline(false)
  , importEndline(false)
  , importSubject(false)
  , m_logInstance(Log::GetLog().RegisterInstance("ImportWidget"))
{
  QHBoxLayout *layout = new QHBoxLayout();

  m_category->addItem("");
  m_category->addItem("Angebot");
  m_category->addItem("Baustelle");
  m_category->addItem("Rechnung");
  
  connect(m_category, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImportWidget::SetIds);
  connect(m_data->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection &selected, const QItemSelection &)
  {
    auto list = selected.indexes();
    if (list.size() == 0)
    {
      return;
    }
    QString prefix = "";
    if (chosenTab == TabName::InvoiceTab)
    {
      prefix = "R";
    }
    else if (chosenTab == TabName::JobsiteTab)
    {
      prefix = "BA";
    }
    else if (chosenTab == TabName::OfferTab)
    {
      prefix = "A";
    }
    chosenId = prefix + m_data->item(list[0].row(), 0)->text();
  });

  QCheckBox *importAddressCheck = new QCheckBox("Importiere Anschrift?", this);
  connect(importAddressCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importAddress = (index == 2);
  });
  importAddressCheck->setChecked(true);
  QCheckBox *importHeadlineCheck = new QCheckBox("Importiere Kopfzeile?", this);
  connect(importHeadlineCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importHeadline = (index == 2);
  });
  importHeadlineCheck->setChecked(true);
  QCheckBox *importEndlineCheck = new QCheckBox("Importiere Endzeile?", this);
  connect(importEndlineCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importEndline = (index == 2);
  });
  QCheckBox *importSubjectCheck = new QCheckBox("Importiere Betreff?", this);
  connect(importSubjectCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importSubject = (index == 2);
  });
  importSubjectCheck->setChecked(true);
  QVBoxLayout *checkLayout = new QVBoxLayout();
  checkLayout->addWidget(importAddressCheck);
  checkLayout->addWidget(importHeadlineCheck);
  checkLayout->addWidget(importEndlineCheck);
  checkLayout->addWidget(importSubjectCheck);

  QFont labelFont("Times", 12, QFont::Bold);

  QGridLayout *dataLayout = new QGridLayout;
  QLabel *categoryText = new QLabel("Typ", this);
  categoryText->setAlignment(Qt::AlignCenter);
  categoryText->setFont(labelFont);
  dataLayout->addWidget(categoryText, 0, 0);
  dataLayout->addWidget(m_category, 1, 0);

  m_data->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_data->setColumnCount(4);
  m_data->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  m_data->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  m_data->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  m_data->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

  QLabel *artNrText = new QLabel("Daten", this);
  artNrText->setFont(labelFont);
  artNrText->setAlignment(Qt::AlignCenter);
  dataLayout->addWidget(artNrText, 0, 1);
  dataLayout->addWidget(m_data, 1, 1);

  layout->addLayout(dataLayout);
  layout->addLayout(checkLayout);

  m_layout->insertLayout(0, layout);
}

ImportWidget::~ImportWidget()
{}

// category = 1 => offer, category = 2 => jobsite, category = 3 => invoice
void ImportWidget::SetIds(int category)
{
  m_data->clear();
  for (int i{}; i < m_data->rowCount(); ++i)
  {
    m_data->removeRow(0);
  }
  if (category == 0)
  {
    return;
  }
  auto &overwatch = Overwatch::GetInstance();
  BaseTab *chosenCategory = nullptr;
  if (category == 1)
  {
    chosenTab = TabName::OfferTab;
  }
  else if (category == 2)
  {
    chosenTab = TabName::JobsiteTab;
  }
  else if (category == 3)
  {
    chosenTab = TabName::InvoiceTab;
  }
  else
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logInstance, "Bad import index chosen");
    return;
  }
  chosenCategory = overwatch.GetTabPointer(chosenTab);
  
  auto data = chosenCategory->GetRowData({ "RENR", "REDAT", "NAME", "GESAMT" });
  int rowCount{};
  for (int i{}; i < data["RENR"].size(); ++i)
  {
    size_t number = data["RENR"][i].toULongLong();
    if (number > 900000)
    {
      continue;
    }
    m_data->insertRow(rowCount);
    m_data->setItem(rowCount, 0, new QTableWidgetItem(data["RENR"][i]));
    m_data->setItem(rowCount, 1, new QTableWidgetItem(data["REDAT"][i]));
    m_data->setItem(rowCount, 2, new QTableWidgetItem(data["NAME"][i]));
    m_data->setItem(rowCount, 3, new QTableWidgetItem(data["GESAMT"][i]));
    ++rowCount;
  }
  m_data->horizontalHeader()->setEnabled(true);
  m_data->verticalHeader()->setVisible(false);
  m_data->setEditTriggers(QTableView::NoEditTriggers);
  m_data->setHorizontalHeaderLabels({ "Nummer", "Datum", "Name", "Netto" });
  m_data->scrollToBottom();
}

void ImportWidget::keyPressEvent(QKeyEvent *e)
{
  if (e->key() != Qt::Key_Escape)
  {
    QDialog::keyPressEvent(e);
  }
  else
  {
    emit Close();
  }
}


CustomSortFilterProxyModel::CustomSortFilterProxyModel(QWidget *parent)
  : QSortFilterProxyModel(parent)
{}

bool CustomSortFilterProxyModel::lessThan(QModelIndex const &left, QModelIndex const &right) const
{
  if (left.column() == 0 && right.column() == 0)
  {
    auto const lhs = left.data().toString().toStdString();
    auto const rhs = right.data().toString().toStdString();
    auto const commaPosLeft = lhs.find(".");
    auto const commaPosRight = rhs.find(".");

    try
    {
      auto const lhsInt = std::stoll(lhs);
      auto const rhsInt = std::stoll(rhs);
      if (lhsInt != 0 && rhsInt != 0 && commaPosLeft == std::string::npos && commaPosRight == std::string::npos)
      {
        if (lhsInt > 900000 && rhsInt < 900000)
        {
          return true;
        }
        if (lhsInt < 900000 && rhsInt > 900000)
        {
          return false;
        }
        return lhsInt < rhsInt;
      }
    }
    catch(...)
    { }

    if (commaPosLeft == std::string::npos && commaPosRight == std::string::npos)
    {
      return QSortFilterProxyModel::lessThan(left, right);
    }

    try
    {
      util::TablePosNumber<1> posLeft(lhs);
      util::TablePosNumber<1> posRight(rhs);
      return posLeft < posRight;
    }
    catch (...)
    {
      return QSortFilterProxyModel::lessThan(left, right);
    }
  }
  else if (left.column() == 1 && right.column() == 1)
  {
    auto const lhs = left.data().toString().toStdString();
    auto const rhs = right.data().toString().toStdString();
    auto dotPosLeft1 = lhs.find(".");
    auto dotPosLeft2 = lhs.find(".", dotPosLeft1 + 1);
    auto dotPosRight1 = rhs.find(".");
    auto dotPosRight2 = rhs.find(".", dotPosRight1 + 1);
    if (dotPosLeft1 == 0 || dotPosLeft2 == 0 || dotPosRight1 == 0 || dotPosRight2 == 0)
    {
      return QSortFilterProxyModel::lessThan(left, right);
    }
    try
    {
      return TablePosDate(lhs, dotPosLeft1, dotPosLeft2) < TablePosDate(rhs, dotPosRight1, dotPosRight2);
    }
    catch(...)
    {
      return QSortFilterProxyModel::lessThan(left, right);
    }
  }
  else
  {
    return QSortFilterProxyModel::lessThan(left, right);
  }
}


PageTextEdit::PageTextEdit(QWidget *parent)
  : QTextEdit(parent)
  , m_parent(static_cast<ParentPage*>(parent))
{}

void PageTextEdit::keyPressEvent(QKeyEvent *ev)
{
  if ((ev->key() == Qt::Key_Return && ev->modifiers() != Qt::Modifier::ALT)
    || (ev->key() == Qt::Key_Enter && ev->modifiers() != (Qt::KeyboardModifier::AltModifier + Qt::KeyboardModifier::KeypadModifier)))
  {
    m_parent->keyPressEvent(ev);
    return;
  }
  if ((ev->key() == Qt::Key_Return && ev->modifiers() == Qt::Modifier::ALT)
    || (ev->key() == Qt::Key_Enter && ev->modifiers() == (Qt::KeyboardModifier::AltModifier + Qt::KeyboardModifier::KeypadModifier)))
  {
    ev->setModifiers(Qt::KeyboardModifier::NoModifier);
    QTextEdit::keyPressEvent(ev);
    return;
  }
  QTextEdit::keyPressEvent(ev);
}

CustomTable::CustomTable(QString const &titleText,
  size_t numberRows,
  QStringList const &columns,
  QWidget *parent)
  : m_table(new QTableWidget((int)numberRows, (int)columns.size()))
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  QFont labelFont("Times", 12, QFont::Bold);
  QLabel *title = new QLabel(titleText, this);
  title->setAlignment(Qt::AlignCenter);
  title->setFont(labelFont);

  m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  for (size_t i = 2; i < columns.size(); ++i)
  {
    m_table->horizontalHeader()->setSectionResizeMode((int)i, QHeaderView::ResizeToContents);
  }
  m_table->horizontalHeader()->setEnabled(true);
  m_table->verticalHeader()->setVisible(false);
  m_table->setHorizontalHeaderLabels(columns);
  m_table->setEditTriggers(QTableView::NoEditTriggers);
  connect(m_table, &QTableWidget::cellDoubleClicked, [this](int row, int)
  {
    QString key = m_table->item(row, 0)->text();
    emit SetSelected(key);
  });

  connect(new QShortcut(QKeySequence(Qt::Key_Escape), this), &QShortcut::activated, [this]()
  {
    emit Close();
  });

  mainLayout->addWidget(title);
  mainLayout->addWidget(m_table);
  setLayout(mainLayout);
}

void CustomTable::SetColumn(size_t column, std::vector<QString> const &data)
{
  for (int i{}; i < (int)data.size(); ++i)
  {
    m_table->setItem(i, (int)column, new QTableWidgetItem(data[i]));
  }
}
