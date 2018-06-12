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
    data["ARTNR"].entry = txt;
  });
  connect(m_ui->editMainDescription, &QLineEdit::textChanged, [this](QString txt)
  {
    data["HAUPTARTBEZ"].entry = txt;
  });
  connect(m_ui->editDescr, &QTextEdit::textChanged, [this]()
  {
    data["ARTBEZ"].entry = m_ui->editDescr->toPlainText();
  });
  connect(m_ui->editUnit, &QLineEdit::textChanged, [this](QString txt)
  {
    data["ME"].entry = txt;
  });
  connect(m_ui->editNetto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const netto = l.toDouble(txt);
    double const brutto = netto + m_mwst / 100 * netto;
    data["NETTO"].entry = netto; 
    data["BRUTTO"].entry = brutto;
    m_ui->editEkp->setText(l.toString(netto, 'f', 2));
    m_ui->editBrutto->setText(l.toString(brutto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editBrutto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German); 
    data["BRUTTO"].entry = l.toDouble(txt);
    //data.netto = data.brutto * 100 / (100 + m_mwst);
    //m_ui->editNetto->setText(l.toString(data.netto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["EKP"].entry = l.toDouble(txt);
  });
  connect(m_ui->editMinutes, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["BAUZEIT"].entry = l.toDouble(txt);
    Calculate();
  });

  connect(new QShortcut(QKeySequence(Qt::Key_F1), this), &QShortcut::activated, this, &MaterialContent::Copy);
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &MaterialContent::Copy);
  if (edit.size() > 0)
  {
    CopyData(edit);
    Calculate();
  }
}

void MaterialContent::Calculate()
{
  QLocale l(QLocale::German);
  double value = data.GetDoubleIfAvailable("NETTO") + data.GetDoubleIfAvailable("BAUZEIT") / 60.0 * m_hourlyRate;
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
}

void MaterialContent::Copy()
{
  try
  {
    QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM MATERIAL";
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
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
  CATCHANDLOGERROR
}

void MaterialContent::CopyData(QString txt)
{
  try
  {
    if (txt.size() == 0)
    {
      return;
    }

    auto tab = Overwatch::GetInstance().GetTabPointer(TabName::MaterialTab);
    if (!tab)
    {
      throw std::runtime_error("Bad TabName for material tab");
    }

    data = tab->GetData(txt.toStdString());
    QLocale l(QLocale::German);
    m_ui->editKey->setText(data.GetString("ARTNR"));
    m_ui->editMainDescription->setText(data.GetString("HAUPTARTBEZ"));
    m_ui->editDescr->setText(data.GetString("ARTBEZ"));
    m_ui->editUnit->setText(data.GetString("ME"));
    m_ui->editMinutes->setText(l.toString(data.GetDouble("BAUZEIT"), 'f', 2));
    m_ui->editNetto->setText(l.toString(data.GetDouble("NETTO"), 'f', 2));
    m_ui->editBrutto->setText(l.toString(data.GetDouble("BRUTTO"), 'f', 2));
    m_ui->editEkp->setText(l.toString(data.GetDouble("EKP"), 'f', 2));
  }
  CATCHANDLOGERROR
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

DatabaseData MaterialPage::GetData() const
{
  return content->data;
}
