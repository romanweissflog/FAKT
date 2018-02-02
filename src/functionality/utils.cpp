#include "functionality\utils.h"
#include "functionality\overwatch.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qlineedit.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qcheckbox.h"
#include "QtWidgets\qshortcut.h"

#include <iostream>

namespace
{
  template<size_t Size>
  struct TablePosNumber
  {
    int64_t integral;
    TablePosNumber<Size - 1> fractional;

    TablePosNumber(std::string const &val)
      : integral(0)
      , fractional("0")
    {
      auto getIntegralPart = [](std::string const &input) -> int32_t
      {
        auto pos = input.find(".");
        if (pos == std::string::npos)
        {
          return std::stoll(input);
        }
        return std::stoll(input.substr(0, pos));
      };
      auto getFractionalPart = [](std::string const &input) -> TablePosNumber<Size - 1>
      {
        auto pos = input.find(".");
        if (pos == std::string::npos || pos == input.size() - 1)
        {
          return TablePosNumber<Size - 1>("0");
        }
        return TablePosNumber<Size - 1>(input.substr(pos + 1, input.size() - pos - 1));
      };
      integral = getIntegralPart(val);
      fractional = getFractionalPart(val);
    }
  };

  template<>
  struct TablePosNumber<0>
  {
    int64_t integral;
    int64_t fractional;

    TablePosNumber(std::string const &val = "")
    {
      integral = std::stoll(val);
      fractional = 0;
    }
  };

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

  template<size_t SizeL, size_t SizeR>
  bool operator<(TablePosNumber<SizeL> const &lhs, TablePosNumber<SizeR> const &rhs)
  {
    if (lhs.integral < rhs.integral)
    {
      return true;
    }
    else if (lhs.integral > rhs.integral)
    {
      return false;
    }
    return lhs.fractional < rhs.fractional;
  }

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
   
  template<size_t Size>
  std::ostream& operator<<(std::ostream &stream, TablePosNumber<Size> const &t)
  {
    stream << t.integral << "." << t.fractional;
    return stream;
  }
}


namespace util
{
  QMessageBox* GetDeleteMessage(QWidget *parent)
  {
    QMessageBox *question = new QMessageBox(parent);
    question->setWindowTitle("WARNUNG");
    question->setText("Wollen sie den Eintrag entfernen?");
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
  this->show();
}

SearchFilter::~SearchFilter()
{}

ShowValue::ShowValue(QString value, QWidget *parent)
  : Entry(parent)
{
  QLabel *label = new QLabel(value, this);

  m_layout->insertWidget(0, label);
  this->show();
}

ShowValue::~ShowValue()
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
  , m_ids(new QComboBox(this))
  , chosenTab(TabName::UndefTab)
  , chosenId("")
  , importAddress(false)
  , importHeadline(false)
  , importEndline(false)
  , importSubject(false)
{
  QHBoxLayout *layout = new QHBoxLayout();

  m_category->addItem("");
  m_category->addItem("Angebot");
  m_category->addItem("Baustelle");
  m_category->addItem("Rechnung");

  connect(m_category, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImportWidget::SetIds);
  connect(m_ids, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), [this](QString txt)
  {
    std::string prefix = "";
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
    chosenId = prefix + txt.toStdString();
  });

  QCheckBox *importAddressCheck = new QCheckBox("Importiere Anschrift?", this);
  connect(importAddressCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importAddress = (index == 2);
  });
  QCheckBox *importHeadlineCheck = new QCheckBox("Importiere Kopfzeile?", this);
  connect(importHeadlineCheck, &QCheckBox::stateChanged, [this](int index)
  {
    importHeadline = (index == 2);
  });
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
  QVBoxLayout *checkLayout = new QVBoxLayout();
  checkLayout->addWidget(importAddressCheck);
  checkLayout->addWidget(importHeadlineCheck);
  checkLayout->addWidget(importEndlineCheck);
  checkLayout->addWidget(importSubjectCheck);

  QFont labelFont("Times", 12, QFont::Bold);

  QVBoxLayout *categoryLayout = new QVBoxLayout;
  QLabel *categoryText = new QLabel("Typ", this);
  categoryText->setFont(labelFont);
  categoryLayout->addWidget(categoryText);
  categoryLayout->addWidget(m_category);

  QVBoxLayout *artNrLayout = new QVBoxLayout;
  QLabel *artNrText = new QLabel("Typ", this);
  artNrText->setFont(labelFont);
  artNrLayout->addWidget(artNrText);
  artNrLayout->addWidget(m_ids);

  layout->addLayout(categoryLayout);
  layout->addLayout(artNrLayout);
  layout->addLayout(checkLayout);

  m_layout->insertLayout(0, layout);
}

ImportWidget::~ImportWidget()
{}

// category = 1 => offer, category = 2 => jobsite, category = 3 => invoice
void ImportWidget::SetIds(int category)
{
  m_ids->clear();
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
    throw std::runtime_error("Bad import index chosen");
  }
  chosenCategory = overwatch.GetTabPointer(chosenTab);
  
  auto ids = chosenCategory->GetArtNumbers();
  m_ids->addItem("");
  for (auto &&i : ids)
  {
    m_ids->addItem(i);
  }
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


MaterialOrService::MaterialOrService(QWidget *parent)
  : Entry(parent)
  , chosenTab(TabName::UndefTab)
{
  QPushButton *material = new QPushButton("Material (M)");
  connect(material, &QPushButton::clicked, [this]()
  {
    chosenTab = TabName::MaterialTab;
    accept();
  });
  QPushButton *service = new QPushButton("Leistung (L)");
  connect(service, &QPushButton::clicked, [this]()
  {
    chosenTab = TabName::ServiceTab;
    accept();
  });
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(material);
  layout->addWidget(service);

  connect(new QShortcut(QKeySequence(Qt::Key_M), this), &QShortcut::activated, [this]() 
  {
    chosenTab = TabName::MaterialTab;
    accept();
  });
  connect(new QShortcut(QKeySequence(Qt::Key_L), this), &QShortcut::activated, [this]()
  {
    chosenTab = TabName::ServiceTab;
    accept();
  });

  m_layout->insertLayout(0, layout);
  show();
}

MaterialOrService::~MaterialOrService()
{}


CustomSortFilterProxyModel::CustomSortFilterProxyModel(QWidget *parent)
  : QSortFilterProxyModel(parent)
{}

bool CustomSortFilterProxyModel::lessThan(QModelIndex const &left, QModelIndex const &right) const
{
  if (left.column() == 0 && right.column() == 0)
  {
    auto const lhs = left.data().toString().toStdString();
    auto const rhs = right.data().toString().toStdString();
    if (lhs.find(".") == std::string::npos && rhs.find(".") == std::string::npos)
    {
      return QSortFilterProxyModel::lessThan(left, right);
    }

    try
    {
      TablePosNumber<5> posLeft(lhs);
      TablePosNumber<5> posRight(rhs);
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
