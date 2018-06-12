#include "pages\address_page.h"
#include "functionality\overwatch.h"

#include "QtSql\qsqlerror.h"
#include "QtCore\qdebug.h"
#include "QtWidgets\qshortcut.h"

#include "ui_address_content.h"
#include "ui_page_framework.h"

AddressContent::AddressContent(Settings *settings,
  QString const &edit,
  QWidget *parent)
  : ParentPage("AddressPage", parent)
  , m_ui(new Ui::addressContent)
  , m_query(*Overwatch::GetInstance().GetDatabase())
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editSearch, &QLineEdit::textChanged, [this](QString txt)
  {
    data["SUCHNAME"].entry = txt;
  });
  connect(m_ui->editNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data["KUNR"].entry = txt.toUInt();
  });
  connect(m_ui->editSalution, &QLineEdit::textChanged, [this](QString txt)
  {
    data["ANREDE"].entry = txt;
  });
  connect(m_ui->editName, &QTextEdit::textChanged, [this]()
  {
    data["NAME"].entry = m_ui->editName->toPlainText();
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    data["STRASSE"].entry = txt;
  });
  connect(m_ui->editCity, &QLineEdit::textChanged, [this](QString txt)
  {
    data["ORT"].entry = txt;
  });
  connect(m_ui->editPlz, &QLineEdit::textChanged, [this](QString txt)
  {
    data["PLZ"].entry = txt;
  });
  connect(m_ui->editPhone, &QLineEdit::textChanged, [this](QString txt)
  {
    data["TELEFON"].entry = txt;
  });
  connect(m_ui->editFax, &QLineEdit::textChanged, [this](QString txt)
  {
    data["FAX"].entry = txt;
  });
  connect(m_ui->editMail, &QLineEdit::textChanged, [this](QString txt)
  {
    data["EMAIL"].entry = txt;
  });

  connect(new QShortcut(QKeySequence(Qt::Key_F1), this), &QShortcut::activated, this, &AddressContent::Copy);
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &AddressContent::Copy);
  if (edit.size() > 0)
  {
    CopyData(edit);
  }
  else
  {
    QString lastCustomer = QString::number(std::stoul(settings->lastCustomer) + 1);
    numberForSettings.emplace(lastCustomer);
    m_ui->editNumber->setText(lastCustomer);
  }
}

void AddressContent::Copy()
{
  try
  {
    QString const sql = "SELECT SUCHNAME, NAME, STRASSE, ORT FROM ADRESSEN";
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
      return;
    }
    std::vector<QString> keys, names, streets, places;
    while (m_query.next())
    {
      keys.push_back(m_query.value(0).toString());
      names.push_back(m_query.value(1).toString());
      streets.push_back(m_query.value(2).toString());
      places.push_back(m_query.value(3).toString());
    }
    importPage = new CustomTable("Adresse-Import", keys.size(), { "Suchname", "Name", QString::fromStdString("Stra" + german::ss + "e"), "Ort" }, this);
    importPage->SetColumn(0, keys);
    importPage->SetColumn(1, names);
    importPage->SetColumn(2, streets);
    importPage->SetColumn(3, places);
    importPage->SetSortingEnabled();
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
  CATCHANDLOGERROR
}

void AddressContent::CopyData(QString txt)
{
  try
  {
    if (txt.size() == 0)
    {
      return;
    }

    auto tab = Overwatch::GetInstance().GetTabPointer(TabName::AddressTab);
    if (!tab)
    {
      throw std::runtime_error("Bad tabname for address");
    }

    data = tab->GetData(txt.toStdString());

    m_ui->editSearch->setText(data.GetString("SUCHNAME"));
    m_ui->editNumber->setText(data.GetString("KUNR"));
    m_ui->editSalution->setText(data.GetString("ANREDE"));
    m_ui->editName->setText(data.GetString("NAME"));
    m_ui->editStreet->setText(data.GetString("STRASSE"));
    m_ui->editPlz->setText(data.GetString("PLZ"));
    m_ui->editCity->setText(data.GetString("ORT"));
    m_ui->editPhone->setText(data.GetString("TELEFON"));
    m_ui->editFax->setText(data.GetString("FAX"));
    m_ui->editMail->setText(data.GetString("EMAIL"));
  }
  CATCHANDLOGERROR
}

void AddressContent::SetFocusToFirst()
{
  m_ui->editSearch->setFocus();
}

AddressPage::AddressPage(Settings *settings,
  QString const &edit,
  QWidget *parent)
  : PageFramework(parent)
  , content(new AddressContent(settings, edit, this))
  , m_settings(settings)
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

void AddressPage::HandleBeforeAccept()
{
  if (content->numberForSettings)
  {
    m_settings->lastCustomer = content->numberForSettings->toStdString();
  }
}

DatabaseData AddressPage::GetData() const
{
  return content->data;
}

