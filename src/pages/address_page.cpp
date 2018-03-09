#include "pages\address_page.h"

#include "QtSql\qsqlerror.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qshortcut.h"

#include "ui_address_content.h"
#include "ui_page_framework.h"

AddressContent::AddressContent(Settings *settings,
  QSqlQuery &query,
  QString const &number,
  QString const &edit,
  QWidget *parent)
  : ParentPage("AddressPage", parent)
  , m_ui(new Ui::addressContent)
  , m_query(query)
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editSearch, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
  });
  connect(m_ui->editNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data.number = txt.toUInt();
  });
  connect(m_ui->editSalution, &QLineEdit::textChanged, [this](QString txt)
  {
    data.salutation = txt;
  });
  connect(m_ui->editName, &QTextEdit::textChanged, [this]()
  {
    data.name = m_ui->editName->toPlainText();
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    data.street = txt;
  });
  connect(m_ui->editCity, &QLineEdit::textChanged, [this](QString txt)
  {
    data.city = txt;
  });
  connect(m_ui->editPlz, &QLineEdit::textChanged, [this](QString txt)
  {
    data.plz = txt;
  });
  connect(m_ui->editPhone, &QLineEdit::textChanged, [this](QString txt)
  {
    data.phone = txt;
  });
  connect(m_ui->editFax, &QLineEdit::textChanged, [this](QString txt)
  {
    data.fax = txt;
  });
  connect(m_ui->editMail, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mail = txt;
  });
  m_ui->editNumber->setText(number);

  connect(new QShortcut(QKeySequence(Qt::Key_F1), this), &QShortcut::activated, this, &AddressContent::Copy);
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &AddressContent::Copy);
  if (edit.size() > 0)
  {
    CopyData(edit);
  }
}

AddressContent::~AddressContent()
{}

void AddressContent::Copy()
{
  QString sql = "SELECT SUCHNAME, NAME FROM ADRESSEN";
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<QString> keys, names;
  while (m_query.next())
  {
    keys.push_back(m_query.value(0).toString());
    names.push_back(m_query.value(1).toString());
  }
  importPage = new CustomTable("Adresse-Import", keys.size(), { "Suchname", "Name" }, this);
  importPage->SetColumn(0, keys);
  importPage->SetColumn(1, names);
  emit AddPage();
  connect(importPage, &CustomTable::SetSelected, [this](QString const &key)
  {
    CopyData(key);
    emit ClosePage();
  });
  connect(importPage, &CustomTable::Close, [this]()
  {
    emit ClosePage();
  });
}

void AddressContent::CopyData(QString txt)
{
  if (txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.next();
  m_ui->editSearch->setText(m_query.value(1).toString());
  m_ui->editNumber->setText(m_query.value(2).toString());
  m_ui->editSalution->setText(m_query.value(3).toString());
  m_ui->editName->setText(m_query.value(4).toString());
  m_ui->editStreet->setText(m_query.value(5).toString());
  m_ui->editPlz->setText(m_query.value(6).toString());
  m_ui->editCity->setText(m_query.value(7).toString());
  m_ui->editPhone->setText(m_query.value(8).toString());
  m_ui->editFax->setText(m_query.value(9).toString());
  m_ui->editMail->setText(m_query.value(10).toString());
}

void AddressContent::SetFocusToFirst()
{
  m_ui->editSearch->setFocus();
}

AddressPage::AddressPage(Settings *settings,
  QSqlQuery &query,
  QString const &number,
  QString const &edit,
  QWidget *parent)
  : PageFramework(parent)
  , content(new AddressContent(settings, query, number, edit, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();

  connect(content, &AddressContent::AddPage, [this]()
  {
    emit AddExtraPage(content->importPage, "Import");
  });
  connect(content, &AddressContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Import");
    content->setFocus();
  });
}

AddressPage::~AddressPage()
{}
