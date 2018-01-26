#include "adding_pages.h"
#include "overwatch.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"

#include "ui_service_page.h"
#include "ui_material_page.h"
#include "ui_address_page.h"
#include "ui_general_page.h"
#include "ui_general_main_page.h"

#include <iostream>


ServicePage::ServicePage(Settings *settings, QSqlQuery &query, QWidget *parent)
  : ParentPage("ServicePage", parent)
  , m_ui(new Ui::servicePage)
  , m_euroPerMin(settings->hourlyRate / 60.0)
  , m_query(query)
{
  m_ui->setupUi(this);
  m_ui->editDescr->setTabChangesFocus(true);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
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
    data.minutes = txt.toDouble();
    double price = data.minutes * m_euroPerMin;
    data.service = price;
    m_ui->labelServicePrice->setText(QString::number(price));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.material = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helperMaterial = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
  });

  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT ARTNR FROM LEISTUNG"))
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
}

ServicePage::~ServicePage()
{}

void ServicePage::Calculate()
{
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void ServicePage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editDescr->setText(m_query.value(2).toString());
  m_ui->editMatPrice->setText(m_query.value(3).toString());
  m_ui->editServicePeriod->setText(m_query.value(4).toString());
  m_ui->labelServicePrice->setText(m_query.value(5).toString());
  m_ui->editHelperMatPrice->setText(m_query.value(6).toString());
  m_ui->labelTotal->setText(m_query.value(7).toString());
  m_ui->editUnit->setText(m_query.value(8).toString());
  m_ui->editMatEkp->setText(m_query.value(9).toString());
}


MaterialPage::MaterialPage(Settings *settings, QSqlQuery &query, QWidget *parent)
  : ParentPage("MaterialPage", parent)
  , m_ui(new Ui::materialPage)
  , m_mwst(settings->mwst)
  , m_query(query)
{
  m_ui->setupUi(this);
  m_ui->editDescr->setTabChangesFocus(true);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
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
    data.netto = txt.toDouble();
    data.brutto = data.netto + m_mwst/100*data.netto;
    m_ui->editBrutto->setText(QString::number(data.brutto));
    Calculate();
  });
  connect(m_ui->editBrutto, &QLineEdit::textChanged, [this](QString txt)
  {
    data.brutto = txt.toDouble();
    data.netto = data.brutto * 100 / (100 + m_mwst);
    m_ui->editNetto->setText(QString::number(data.netto));
    Calculate();
  });
  connect(m_ui->editEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
    Calculate();
  });

  m_query.clear();
  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT ARTNR FROM MATERIAL"))
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
}

MaterialPage::~MaterialPage()
{}

void MaterialPage::Calculate()
{
  double value = data.netto;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void MaterialPage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editDescr->setText(m_query.value(2).toString());
  m_ui->editUnit->setText(m_query.value(3).toString());
  m_ui->editEkp->setText(m_query.value(4).toString());
  m_ui->editNetto->setText(m_query.value(5).toString());
  m_ui->editMinutes->setText(m_query.value(6).toString());
  m_ui->editBrutto->setText(m_query.value(7).toString());
  m_ui->labelTotal->setText(m_query.value(8).toString());
}


AddressPage::AddressPage(Settings *settings, 
  QSqlQuery &query, 
  QString edit,
  QWidget *parent)
  : ParentPage("AddressPage", parent)
  , m_ui(new Ui::addressPage)
  , m_query(query)
{
  m_ui->setupUi(this);
  m_ui->editName->setTabChangesFocus(true);
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

  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT SUCHNAME FROM ADRESSEN"))
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
  if (edit.size() > 0)
  {
    CopyData(edit);
  }
}

AddressPage::~AddressPage()
{}

void AddressPage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0 && txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM ADRESSEN WHERE SUCHNAME = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
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
}


GeneralPage::GeneralPage(Settings *settings,
  uint64_t number,
  std::string const &lastPos,
  QSqlQuery &query,
  QWidget *parent)
  : ParentPage("GeneralPage", parent)
  , m_ui(new Ui::generalPage)
  , m_query(query)
  , m_hourlyRate(settings->hourlyRate)
{
  m_ui->setupUi(this);
  m_ui->editText->setTabChangesFocus(true);
  QSqlQueryModel *model = new QSqlQueryModel(this);
  model->setQuery(m_query);

  m_ui->labelNr->setText(QString::number(number));
  m_ui->editPos->setText(QString::fromStdString(lastPos));
  m_ui->labelGenRate->setText(QString::number(m_hourlyRate));

  data = {};
  SetConnections();
}

void GeneralPage::CopyData(uint64_t number, std::string const &pos)
{
  if (!m_query.prepare("SELECT * FROM " + QString::number(number) + " WHERE POSIT = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(pos));
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  m_ui->editArtNr->setText(m_query.value(1).toString());
  m_ui->editText->setText(m_query.value(2).toString());
  m_ui->editUnitType->setText(m_query.value(3).toString());
  m_ui->editUnitSize->setText(m_query.value(4).toString());
  m_ui->labelEP->setText(m_query.value(5).toString());
  m_ui->editServicePrice->setText(m_query.value(6).toString());
  m_ui->editHelpMat->setText(m_query.value(7).toString());
  m_ui->labelPriceTotal->setText(m_query.value(8).toString());
  m_ui->editServiceTime->setText(m_query.value(9).toString());
  m_ui->editMaterialDiscount->setText(m_query.value(14).toString());
  m_ui->editArtNr->setText(m_query.value(16).toString());
  m_ui->editServiceRate->setText(m_query.value(17).toString());
  m_ui->editMaterialEKP->setText(m_query.value(18).toString());
}

GeneralPage::~GeneralPage()
{}

void GeneralPage::SetConnections()
{
  connect(m_ui->editPos, &QLineEdit::textChanged, [this](QString txt)
  {
    data.pos = txt;
  });
  connect(m_ui->editArtNr, &QLineEdit::textChanged, [this](QString txt)
  {
    data.artNr = txt;
  });
  connect(m_ui->editText, &QTextEdit::textChanged, [this]()
  {
    data.text = m_ui->editText->toPlainText();;
  });
  connect(m_ui->editUnitType, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editUnitSize, &QLineEdit::textChanged, [this](QString txt)
  {
    data.number = txt.toULong();
    Calculate();
  });
  connect(m_ui->editMaterialEKP, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMaterialSurchage, &QLineEdit::textChanged, [this](QString txt)
  {
    data.surcharge = txt.toDouble();
    double matPrice = (100.0 + data.surcharge) / 100.0*data.ekp;
    m_ui->editMaterialPrice->setText(QString::number(matPrice));
  });
  connect(m_ui->editMaterialPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.material = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMaterialDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServiceRate, &QLineEdit::textChanged, [this](QString txt)
  {
    data.hourlyRate = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServiceTime, &QLineEdit::textChanged, [this](QString txt)
  {
    data.time = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServicePrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.service = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helpMat = txt.toDouble();
    Calculate();
  });
}

void GeneralPage::Calculate()
{
  double surchage = (data.ekp == 0 ? -100.0 : (data.material - data.ekp) / data.ekp * 100);
  m_ui->editMaterialSurchage->setText(QString::number(surchage));

  double profitMatPerc = (data.ekp == 0 ? 100.0 : (data.material - data.ekp) / data.material * 100);
  m_ui->labelProfitMatPerc->setText(QString::number(profitMatPerc));

  double profitMatEur = (data.material - data.ekp)*data.number;
  m_ui->labelProfitMatEur->setText(QString::number(profitMatEur));
  m_ui->labelWorkingHours->setText(QString::number(data.number*data.time));

  double matPrice = (100.0 - data.discount) / 100.0 * data.material;
  m_ui->labelMaterialQuant->setText(QString::number(matPrice));

  double servicePrice = data.time / 60.0*data.hourlyRate;
  m_ui->editServicePrice->setText(QString::number(servicePrice));

  double ep = matPrice + servicePrice + data.helpMat;
  m_ui->labelEP->setText(QString::number(ep));
  data.ep = ep;
  m_ui->labelPriceTotal->setText(QString::number(ep*data.number));
  data.total = ep*data.number;
}

void GeneralPage::TakeFromMaterial()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::MaterialTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto data = static_cast<MaterialData*>(tab->GetData(chosenArtNr.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Material data not found");
    }
    m_ui->editArtNr->setText(data->key);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(QString::number(data->ekp));
    m_ui->editMaterialPrice->setText(QString::number(data->netto));
  }
}

void GeneralPage::TakeFromService()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::ServiceTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto data = static_cast<ServiceData*>(tab->GetData(chosenArtNr.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Material data not found");
    }
    m_ui->editArtNr->setText(data->key);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(QString::number(data->ekp));
    m_ui->editMaterialPrice->setText(QString::number(data->material));
    m_ui->editServiceTime->setText(QString::number(data->minutes));
    m_ui->editServicePrice->setText(QString::number(data->service));
    m_ui->editHelpMat->setText(QString::number(data->helperMaterial));
  }
}

void GeneralPage::MakeNewEntry()
{

}


GeneralMainPage::GeneralMainPage(Settings *settings, std::string const &number, WindowType const &type, QWidget *parent)
  : ParentPage("OfferPage", parent)
  , m_ui(new Ui::generalMainPage)
  , m_hourlyRate(settings->hourlyRate)
  , m_defaultHeadline(QString::fromStdString(settings->defaultHeadline))
  , m_defaultEndline(QString::fromStdString(settings->defaultEndline))
{
  if (type == WindowType::WIndowTypeOffer)
  {
    m_internalData = new OfferData();
  }
  else
  {
    m_internalData = new InvoiceData();
  }
  m_ui->setupUi(this);
  m_ui->editHeading->setTabChangesFocus(true);
  m_ui->editEnding->setTabChangesFocus(true);

  connect(m_ui->editNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->number = txt;
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    m_internalData->date = txt;
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
  connect(m_ui->buttonHeading, &QPushButton::clicked, [this]()
  {
    m_ui->editHeading->setText(m_defaultHeadline);
  });
  connect(m_ui->buttonEnding, &QPushButton::clicked, [this]()
  {
    m_ui->editEnding->setText(m_defaultEndline);
  });
  m_ui->editNumber->setText(QString::fromStdString(number));
  m_ui->editHourlyRate->setText(QString::number(m_hourlyRate));
}

GeneralMainPage::~GeneralMainPage()
{
  delete m_internalData;
}

void GeneralMainPage::TakeFromAdress()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::AddressTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenCustomer = dia->currentItem;
    auto data = static_cast<AddressData*>(tab->GetData(chosenCustomer.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Adress data not found");
    }
    m_ui->editCustomerNumber->setText(QString::number(data->number));
    m_ui->editSalutation->setText(data->salutation);
    m_ui->editName->setText(data->name);
    m_ui->editStreet->setText(data->street);
    m_ui->editPlace->setText(data->plz + " " + data->city);
  }
}

void GeneralMainPage::SetData(GeneralMainData *data)
{
  m_ui->editNumber->setText(data->number);
  m_ui->editDate->setText(data->date);
  m_ui->editCustomerNumber->setText(data->customerNumber);
  m_ui->editSalutation->setText(data->salutation);
  m_ui->editName->setText(data->name);
  m_ui->editStreet->setText(data->street);
  m_ui->editPlace->setText(data->place);
  m_ui->editSkonto->setText(QString::number(data->skonto));
  m_ui->editPayNormal->setText(QString::number(data->payNormal));
  m_ui->editPaySkonto->setText(QString::number(data->paySkonto));
  m_ui->editHourlyRate->setText(QString::number(data->hourlyRate));
  m_ui->editHeading->setText(data->headline);
  m_ui->editEnding->setText(data->endline);
}

InvoicePage::InvoicePage(Settings *settings, std::string const &invoiceNumber, TabName const &tab, QWidget *parent)
  : GeneralMainPage(settings, invoiceNumber, WindowType::WindowTypeInvoice, parent)
  , data(static_cast<InvoiceData*>(m_internalData))
  , m_mwstEdit(new QLineEdit(this))
  , m_deliveryEdit(new QLineEdit(this))
{
  if (tab == TabName::InvoiceTab)
  {
    this->setWindowTitle("Rechnung");
    m_ui->labelTypeNumber->setText("Rechnungsnummer:");
    m_ui->labelTypeDate->setText("Rechnungsdatum:");
  }
  else if (tab == TabName::JobsiteTab)
  {
    this->setWindowTitle("Baustelle");
    m_ui->labelTypeNumber->setText("Baustellennummer:");
    m_ui->labelTypeDate->setText("Baustelle-Datum:");
  }

  QHBoxLayout *mwstLayout = new QHBoxLayout();
  QLabel *mwstLabel = new QLabel("Mwst. (%):");
  connect(m_mwstEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->mwst = txt.toDouble();
  });
  mwstLayout->addWidget(mwstLabel);
  mwstLayout->addWidget(m_mwstEdit);
  m_ui->specialDataLayout->insertLayout(0, mwstLayout);

  QHBoxLayout *deliveryLayout = new QHBoxLayout();
  QLabel *deliveryLabel = new QLabel("Lieferg. v.:");
  connect(m_deliveryEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->deliveryDate = txt;
  });
  deliveryLayout->addWidget(deliveryLabel);
  deliveryLayout->addWidget(m_deliveryEdit);
  m_ui->specialDataLayout->insertLayout(3, deliveryLayout);

  setTabOrder(m_ui->editSubject, m_mwstEdit);
  setTabOrder(m_mwstEdit, m_ui->editSkonto);
  setTabOrder(m_ui->editSkonto, m_ui->editPayNormal);
  setTabOrder(m_ui->editPayNormal, m_deliveryEdit);
  setTabOrder(m_deliveryEdit, m_ui->editHourlyRate);
  setTabOrder(m_ui->editHourlyRate, m_ui->editHeading);
  setTabOrder(m_ui->editHeading, m_ui->editEnding);
  setTabOrder(m_ui->editEnding, m_ui->buttonHeading);
  setTabOrder(m_ui->buttonHeading, m_ui->buttonEnding);
}

InvoicePage::~InvoicePage()
{}

void InvoicePage::SetData(GeneralMainData *data)
{
  GeneralMainPage::SetData(data);
  InvoiceData *invoiceData = static_cast<InvoiceData*>(data);
  m_mwstEdit->setText(QString::number(invoiceData->mwst));
  m_deliveryEdit->setText(invoiceData->deliveryDate);
}


OfferPage::OfferPage(Settings *settings, std::string const &invoiceNumber, QWidget *parent)
  : GeneralMainPage(settings, invoiceNumber, WindowType::WIndowTypeOffer, parent)
  , data(static_cast<OfferData*>(m_internalData))
  , m_deadLineEdit(new QLineEdit(this))
{
  this->setWindowTitle("Angebot");
  m_ui->labelTypeNumber->setText("Angebotsnummber:");
  m_ui->labelTypeDate->setText("Angebots-Datum:");

  QHBoxLayout *deadLineLayout = new QHBoxLayout();
  QLabel *deadLineLabel = new QLabel("Bindefrist intern:");
  connect(m_deadLineEdit, &QLineEdit::textChanged, [this](QString txt)
  {
    data->deadLine = txt;
  });
  deadLineLayout->addWidget(deadLineLabel);
  deadLineLayout->addWidget(m_deadLineEdit);

  setTabOrder(m_ui->editHourlyRate, m_deadLineEdit);
  setTabOrder(m_deadLineEdit, m_ui->editHeading);
  setTabOrder(m_ui->editHeading, m_ui->editEnding);
  setTabOrder(m_ui->editEnding, m_ui->buttonHeading);
  setTabOrder(m_ui->buttonHeading, m_ui->buttonEnding);

  m_ui->specialDataLayout->insertLayout(3, deadLineLayout);
}

OfferPage::~OfferPage()
{}

void OfferPage::SetData(GeneralMainData *data)
{
  GeneralMainPage::SetData(data);
  OfferData *offerData = static_cast<OfferData*>(data);
  m_deadLineEdit->setText(offerData->deadLine);
}
