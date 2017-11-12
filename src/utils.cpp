#include "utils.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qlineedit.h"
#include "QtWidgets\qlabel.h"

Entry::Entry(QWidget *parent)
  : QDialog(parent)
  , m_buttonBox(new QDialogButtonBox(this))
  , m_layout(new QVBoxLayout())
{
  m_widgets.push_back(m_buttonBox);

  QPushButton *cancelButton = new QPushButton("Cancel", this);
  m_buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  QPushButton *okButton = new QPushButton("OK", this);
  m_buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
  connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

  m_widgets.push_back(cancelButton);
  m_widgets.push_back(okButton);
  m_layout->addWidget(m_buttonBox);
  this->setLayout(m_layout);
}

Entry::~Entry()
{
  //for (auto w : m_widgets)
  //{
  //  delete w;
  //}
}


EditOneEntry::EditOneEntry(QString oldValue, QWidget *parent)
  : Entry(parent)
{
  QLineEdit *lineEdit = new QLineEdit(this);
  connect(lineEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    newValue = txt;
  });
  m_widgets.push_back(lineEdit);

  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *oldValLabel = new QLabel("Alter Wert:");
  QLabel *oldVal = new QLabel(oldValue);
  QLabel *newValLabel = new QLabel("Neuer Wert: ");
  layout->addWidget(oldValLabel);
  layout->addWidget(oldVal);
  layout->addWidget(newValLabel);
  layout->addWidget(lineEdit);
  m_widgets.push_back(oldValLabel);
  m_widgets.push_back(oldVal);
  m_widgets.push_back(newValLabel);

  m_layout->insertLayout(0, layout);

  this->show();
}

EditOneEntry::~EditOneEntry()
{
}