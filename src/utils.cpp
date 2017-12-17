#include "utils.h"
#include "overwatch.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qlineedit.h"
#include "QtWidgets\qlabel.h"
#include "QtWidgets\qcheckbox.h"

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


EditOneEntry::EditOneEntry(QString oldValue, QWidget *parent)
  : Entry(parent)
{
  QLineEdit *lineEdit = new QLineEdit(this);
  connect(lineEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    newValue = txt;
  });

  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *oldValLabel = new QLabel("Alter Wert:");
  QLabel *oldVal = new QLabel(oldValue);
  QLabel *newValLabel = new QLabel("Neuer Wert: ");
  layout->addWidget(oldValLabel);
  layout->addWidget(oldVal);
  layout->addWidget(newValLabel);
  layout->addWidget(lineEdit);

  m_layout->insertLayout(0, layout);

  this->show();
}

EditOneEntry::~EditOneEntry()
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

  layout->addWidget(m_category);
  layout->addWidget(m_ids);
  layout->addLayout(checkLayout);
  m_layout->insertLayout(0, layout);
  this->show();
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
