#include "pages\service_page.h"
#include "functionality\overwatch.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qshortcut.h"

#include "ui_service_content.h"
#include "ui_page_framework.h"


ServiceContent::ServiceContent(Settings *settings,
  QString const &edit,
  QWidget *parent)
  : ParentPage("ServicePage", parent)
  , m_ui(new Ui::serviceContent)
  , m_euroPerMin(util::Precision2Round(settings->hourlyRate / 60.0))
  , m_query(*Overwatch::GetInstance().GetDatabase())
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
  });
  connect(m_ui->editMainDescription, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mainDescription = txt;
  });
  connect(m_ui->editDescr, &QTextEdit::textChanged, [this]()
  {
    data.description = m_ui->editDescr->toPlainText();
  });
  connect(m_ui->editUnit, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editServicePeriod, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.minutes = l.toDouble(txt);
    double price = data.minutes * m_euroPerMin;
    data.service = price;
    m_ui->labelServicePrice->setText(l.toString(price, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.material = l.toDouble(txt);
    m_ui->editMatEkp->setText(l.toString(data.material, 'f', 2));
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.helperMaterial = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.ekp = l.toDouble(txt);
  });

  SHORTCUT(f1Key, Key_F1, Copy)
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &ServiceContent::Copy);
  m_ui->buttonCopy->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  if (edit.size() > 0)
  {
    CopyData(edit);
    Calculate();
  }
}

ServiceContent::~ServiceContent()
{}

void ServiceContent::SetData(GeneralData const &data)
{
  QLocale l(QLocale::German);
  m_ui->editKey->setText(data.artNr);
  m_ui->editMainDescription->setText(data.mainText);
  m_ui->editDescr->setText(data.text);
  m_ui->editUnit->setText(data.unit);
  m_ui->editMatEkp->setText(l.toString(data.ekp, 'f', 2));
  m_ui->editServicePeriod->setText(l.toString(data.time, 'f', 2));
  m_ui->editMatPrice->setText(l.toString(data.material, 'f', 2));
  m_ui->editHelperMatPrice->setText(l.toString(data.helpMat, 'f', 2));
}

void ServiceContent::Calculate()
{
  QLocale l(QLocale::German);
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
  data.ep = value;
}

void ServiceContent::Copy()
{
  QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM LEISTUNG";
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<QString> numbers, mainDescription, descriptions;
  while (m_query.next())
  {
    numbers.push_back(m_query.value(0).toString());
    mainDescription.push_back(m_query.value(1).toString());
    descriptions.push_back(m_query.value(2).toString());
  }
  importPage = new CustomTable("Leistung-Import", numbers.size(), { "Artikelnummer", "Haupt-Bezeichnung", "Extra-Bezeichnung" }, this);
  importPage->SetColumn(0, numbers);
  importPage->SetColumn(1, mainDescription);
  importPage->SetColumn(2, descriptions);
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

void ServiceContent::CopyData(QString txt)
{
  if (txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID"))
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

  QLocale l(QLocale::German);
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editMainDescription->setText(m_query.value(2).toString());
  m_ui->editDescr->setText(m_query.value(3).toString());
  m_ui->editMatPrice->setText(l.toString(m_query.value(4).toDouble(), 'f', 2));
  m_ui->editServicePeriod->setText(l.toString(m_query.value(5).toDouble(), 'f', 2));
  m_ui->labelServicePrice->setText(l.toString(m_query.value(6).toDouble(), 'f', 2));
  m_ui->editHelperMatPrice->setText(l.toString(m_query.value(7).toDouble(), 'f', 2));
  m_ui->labelTotal->setText(l.toString(m_query.value(8).toDouble(), 'f', 2));
  m_ui->editUnit->setText(m_query.value(9).toString());
  m_ui->editMatEkp->setText(l.toString(m_query.value(10).toDouble(), 'f', 2));
}

void ServiceContent::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}


ServicePage::ServicePage(Settings *settings,
  QString const &edit,
  QWidget *parent)
  : PageFramework(parent)
  , content(new ServiceContent(settings, edit, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();
  
  connect(content, &ServiceContent::AddPage, [this]()
  {
    emit AddExtraPage(content->importPage, "Import");
  });
  connect(content, &ServiceContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Import");
    content->setFocus();
  });
}

void ServicePage::SetData(GeneralData const &data)
{
  content->SetData(data);
}

ServicePage::~ServicePage()
{}
