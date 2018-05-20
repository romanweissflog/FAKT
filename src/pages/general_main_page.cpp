#include "pages\general_main_page.h"
#include "functionality\overwatch.h"

#include "QtWidgets\qshortcut.h"
#include "QtSql\qsqlerror.h"

#include "ui_general_main_content.h"

#include <ctime>

namespace
{
  std::string GetChildName(TabName const &type)
  {
    if (type == TabName::OfferTab)
    {
      return "OfferTab";
    }
    else if (type == TabName::InvoiceTab)
    {
      return "InvoiceTab";
    }
    else
    {
      return "JobsiteTab";
    }
  }
}

GeneralMainContent::GeneralMainContent(Settings *settings,
  QString const &number, 
  TabName const &childType,
  QWidget *parent)
  : ParentPage(GetChildName(childType), parent)
  , m_ui(new Ui::generalMainContent)
  , m_hourlyRate(settings->hourlyRate)
  , m_defaultHeadline(settings->defaultHeadline)
{
  m_query = QSqlQuery(*Overwatch::GetInstance().GetDatabase());
  if (childType == TabName::OfferTab)
  {
    m_internalData.reset(new OfferData());
    m_defaultEndline = settings->defaultOfferEndline;
  }
  else
  {
    m_internalData.reset(new InvoiceData());
    m_defaultEndline = settings->defaultInvoiceEndline;
  }
  m_ui->setupUi(this);

  connect(m_ui->editNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->number = txt;
    if (util::IsNumberValid(txt))
    {
      m_ui->labelNumberError->setText("");
    }
    else
    {
      m_ui->labelNumberError->setText(QString::fromStdString("Ung" + german::ue + "ltige Nummer"));
    }
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->date = txt;
    if (util::IsDateValid(txt))
    {
      m_ui->labelDateError->setText("");
    }
    else
    {
      m_ui->labelDateError->setText(QString::fromStdString("Ung" + german::ue + "ltiges Datum"));
    }
  });
  connect(m_ui->editCustomerNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->customerNumber = txt;
  });
  connect(m_ui->editSalutation, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->salutation = txt;
  });
  connect(m_ui->editName, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->name = txt;
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->street = txt;
  });
  connect(m_ui->editPlace, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->place = txt;
  });
  connect(m_ui->editSubject, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->subject = txt;
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->discount = txt.toDouble();
  });
  connect(m_ui->editSkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->skonto = txt.toDouble();
  });
  connect(m_ui->editPayNormal, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->payNormal = txt.toDouble();
  });
  connect(m_ui->editPaySkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->paySkonto = txt.toDouble();
  });
  connect(m_ui->editHourlyRate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->hourlyRate = txt.toDouble();
  });
  connect(m_ui->editHeading, &QTextEdit::textChanged, [this]()
  {
    m_internalData->headline = m_ui->editHeading->toPlainText();;
  });
  connect(m_ui->editEnding, &QTextEdit::textChanged, [this]()
  {
    m_internalData->endline = m_ui->editEnding->toPlainText();;
  });
  m_ui->editNumber->setText(number);
  m_ui->editHourlyRate->setText(QString::number(m_hourlyRate));
  m_ui->editPayNormal->setText("14");
  m_ui->editPaySkonto->setText("5");

  std::time_t t = std::time(0);
  struct tm *now = std::localtime(&t);
  QString day = QString::number(now->tm_mday);
  if (day.size() == 1)
  {
    day = "0" + day;
  }
  QString month = QString::number(now->tm_mon + 1);
  if (month.size() == 1)
  {
    month = "0" + month;
  }
  QString timeDate = day + "."
    + month + "."
    + QString::number(now->tm_year + 1900);
  m_ui->editDate->setText(timeDate);

  m_ui->editEnding->setText(m_defaultEndline);

  new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(TakeFromAdress()));
  connect(new QShortcut(QKeySequence(Qt::Key_F5), this), &QShortcut::activated, [this]()
  {
    m_ui->editHeading->setText(m_defaultHeadline);
  });
  connect(new QShortcut(QKeySequence(Qt::Key_F6), this), &QShortcut::activated, [this]()
  {
    m_ui->editEnding->setText(m_defaultEndline);
  });
}

GeneralMainContent::~GeneralMainContent()
{
}

void GeneralMainContent::SetFocusToFirst()
{
  m_ui->editNumber->setFocus();
}

void GeneralMainContent::TakeFromAdress()
{
  QString sql = "SELECT SUCHNAME, NAME, STRASSE, ORT FROM ADRESSEN";
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

void GeneralMainContent::SetData(GeneralMainData *data)
{
  m_ui->editNumber->setText(data->number);
  m_ui->editDate->setText(data->date);
  m_ui->editCustomerNumber->setText(data->customerNumber);
  m_ui->editSalutation->setText(data->salutation);
  m_ui->editName->setText(data->name);
  m_ui->editStreet->setText(data->street);
  m_ui->editPlace->setText(data->place);
  m_ui->editDiscount->setText(QString::number(data->discount));
  m_ui->editSkonto->setText(QString::number(data->skonto));
  m_ui->editPayNormal->setText(QString::number(data->payNormal));
  m_ui->editPaySkonto->setText(QString::number(data->paySkonto));
  m_ui->editHourlyRate->setText(QString::number(data->hourlyRate));
  m_ui->editSubject->setText(data->subject);
  m_ui->editHeading->setText(data->headline);
  m_ui->editEnding->setText(data->endline);
}

void GeneralMainContent::CopyData(QString const &key)
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::AddressTab);
  auto input = tab->GetData(key.toStdString());
  std::unique_ptr<AddressData> data(static_cast<AddressData*>(input.release()));
  if (data == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Adress data not found for " + key.toStdString());
    return;
  }
  m_ui->editCustomerNumber->setText(QString::number(data->number));
  m_ui->editSalutation->setText(data->salutation);
  m_ui->editName->setText(data->name);
  m_ui->editStreet->setText(data->street);
  m_ui->editPlace->setText(data->plz + " " + data->city);
}

void GeneralMainContent::LockNumber()
{
  m_ui->editNumber->setEnabled(false);
}
