#include "pages\material_page.h"
#include "functionality\overwatch.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qshortcut.h"

#include "ui_material_content.h"
#include "ui_page_framework.h"

MaterialContent::MaterialContent(Settings *settings,
  QString const &edit,
  QWidget *parent)
  : ParentPage("MaterialPage", parent)
  , m_ui(new Ui::materialContent)
  , m_mwst(settings->mwst)
  , m_hourlyRate(settings->hourlyRate)
  , m_query(*Overwatch::GetInstance().GetDatabase())
  , importPage(nullptr)
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
  connect(m_ui->editNetto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.netto = l.toDouble(txt);
    data.brutto = data.netto + m_mwst / 100 * data.netto;
    m_ui->editEkp->setText(l.toString(data.netto, 'f', 2));
    m_ui->editBrutto->setText(l.toString(data.brutto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editBrutto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.brutto = l.toDouble(txt);
    //data.netto = data.brutto * 100 / (100 + m_mwst);
    //m_ui->editNetto->setText(l.toString(data.netto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.ekp = l.toDouble(txt);
  });
  connect(m_ui->editMinutes, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.minutes = l.toDouble(txt);
    Calculate();
  });

  SHORTCUT(f1Key, Key_F1, Copy)
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &MaterialContent::Copy);
  m_ui->buttonCopy->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  if (edit.size() > 0)
  {
    CopyData(edit);
    Calculate();
  }
}

MaterialContent::~MaterialContent()
{}

void MaterialContent::SetData(GeneralData const &data)
{
  QLocale l(QLocale::German);
  m_ui->editKey->setText(data.artNr);
  m_ui->editMainDescription->setText(data.mainText);
  m_ui->editDescr->setText(data.text);
  m_ui->editUnit->setText(data.unit);
  m_ui->editMinutes->setText(l.toString(data.time, 'f', 2));
  m_ui->editEkp->setText(l.toString(data.ekp, 'f', 2));
  m_ui->editNetto->setText(l.toString(data.material, 'f', 2));
  double const brutto = data.material * (100.0 + m_mwst) / 100.0;
  m_ui->editBrutto->setText(l.toString(brutto, 'f', 2));
}

void MaterialContent::Calculate()
{
  QLocale l(QLocale::German);
  double value = data.netto + data.minutes / 60.0 * m_hourlyRate;
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
}

void MaterialContent::Copy()
{
  QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM MATERIAL";
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<QString> numbers, mainDescriptions, descriptions;
  while (m_query.next())
  {
    numbers.push_back(m_query.value(0).toString());
    mainDescriptions.push_back(m_query.value(1).toString());
    descriptions.push_back(m_query.value(2).toString());
  }
  importPage = new CustomTable("Material-Import", numbers.size(), { "Artikelnummer", "Haupt-Bezeichnung", "Extra-Bezeichnung" }, this);
  importPage->SetColumn(0, numbers);
  importPage->SetColumn(1, mainDescriptions);
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

void MaterialContent::CopyData(QString txt)
{
  if (txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID"))
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
  m_ui->editUnit->setText(m_query.value(4).toString());
  m_ui->editNetto->setText(l.toString(m_query.value(6).toDouble(), 'f', 2));
  m_ui->editMinutes->setText(l.toString(m_query.value(7).toDouble(), 'f', 2));
  m_ui->editBrutto->setText(l.toString(m_query.value(8).toDouble(), 'f', 2));
  m_ui->editEkp->setText(l.toString(m_query.value(5).toDouble(), 'f', 2));
  m_ui->labelTotal->setText(l.toString(m_query.value(9).toDouble(), 'f', 2));
}

void MaterialContent::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}

MaterialPage::MaterialPage(Settings *settings, 
  QString const &edit, 
  QWidget *parent)
  : PageFramework(parent)
  , content(new MaterialContent(settings, edit, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();

  connect(content, &MaterialContent::AddPage, [this]()
  {
    emit AddExtraPage(content->importPage, "Import");
  });
  connect(content, &MaterialContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Import");
    content->setFocus();
  });
}

void MaterialPage::SetData(GeneralData const &data)
{
  content->SetData(data);
}

MaterialPage::~MaterialPage()
{}
