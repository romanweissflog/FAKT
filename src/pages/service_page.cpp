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
  , m_euroPerMin(settings->hourlyRate / 60.0)
  , m_query(*Overwatch::GetInstance().GetDatabase())
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
  connect(m_ui->editServicePeriod, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const minutes = l.toDouble(txt);
    double const price = minutes * m_euroPerMin;
    data["LP"].entry = price;
    m_ui->labelServicePrice->setText(l.toString(price, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const material = l.toDouble(txt); 
    data["MP"].entry = material;
    m_ui->editMatEkp->setText(l.toString(material, 'f', 2));
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["SP"].entry = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["EKP"].entry = l.toDouble(txt);
  });

  connect(new QShortcut(QKeySequence(Qt::Key_F1), this), &QShortcut::activated, this, &ServiceContent::Copy);
  connect(m_ui->buttonCopy, &QPushButton::clicked, this, &ServiceContent::Copy);
  if (edit.size() > 0)
  {
    CopyData(edit);
    Calculate();
  }
}

ServiceContent::~ServiceContent()
{}

void ServiceContent::Calculate()
{
  QLocale l(QLocale::German);
  double const value = data.GetDouble("LP") 
    + data.GetDouble("MP") + data.GetDouble("SP");
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
  data["EP"].entry = value;
}

void ServiceContent::Copy()
{
  try
  {
    QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM LEISTUNG";
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
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
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}

void ServiceContent::CopyData(QString txt)
{
  try
  {
    if (txt.size() == 0)
    {
      return;
    }

    auto tab = Overwatch::GetInstance().GetTabPointer(TabName::ServiceTab);
    if (!tab)
    {
      throw std::runtime_error("Bad tabname for service tab");
    }

    auto const data = tab->GetData(txt.toStdString());

    QLocale l(QLocale::German);
    m_ui->editKey->setText(data.GetString("ARTNR"));
    m_ui->editMainDescription->setText(data.GetString("HAUPTARTBEZ"));
    m_ui->editDescr->setText(data.GetString("ARTBEZ"));
    m_ui->editMatPrice->setText(l.toString(data.GetDouble("MP"), 'f', 2));
    m_ui->editServicePeriod->setText(l.toString(data.GetDouble("BAUZEIT"), 'f', 2));
    m_ui->labelServicePrice->setText(l.toString(data.GetDouble("LP"), 'f', 2));
    m_ui->editHelperMatPrice->setText(l.toString(data.GetDouble("SP"), 'f', 2));
    m_ui->labelTotal->setText(l.toString(data.GetDouble("EP"), 'f', 2));
    m_ui->editUnit->setText(data.GetString("ME"));
    m_ui->editMatEkp->setText(l.toString(data.GetDouble("EKP"), 'f', 2));
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
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

ServicePage::~ServicePage()
{}
