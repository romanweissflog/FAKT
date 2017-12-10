#include "adding_pages.h"
#include "overwatch.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtSql\qsqlquerymodel.h"

#include "ui_service_page.h"
#include "ui_material_page.h"
#include "ui_address_page.h"
#include "ui_general_page.h"
#include "ui_invoice_page.h"
#include "ui_offer_page.h"

#include <iostream>

ServicePage::ServicePage(Settings *settings, QSqlQuery &query, QWidget *parent)
  : ParentPage("ServicePage", parent)
  , m_ui(new Ui::servicePage)
  , m_euroPerMin(settings->euroPerMin)
  , m_query(query)
{
  m_ui->setupUi(this);
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
    double price = data.minutes*m_euroPerMin;
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
  m_ui->editStockSize->setText(m_query.value(6).toString());
  m_ui->supplierBox->setCurrentText(m_query.value(7).toString());
  m_ui->editMinutes->setText(m_query.value(8).toString());
  m_ui->editBrutto->setText(m_query.value(9).toString());
  m_ui->labelTotal->setText(m_query.value(10).toString());
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
  connect(m_ui->editPhone1, &QLineEdit::textChanged, [this](QString txt)
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
  m_ui->editPhone1->setText(m_query.value(8).toString());
  m_ui->editFax->setText(m_query.value(9).toString());
  m_ui->labelNetto->setText(m_query.value(14).toString());
  m_ui->labelBrutto->setText(m_query.value(16).toString());
  m_ui->editPhone2->setText(m_query.value(17).toString());
  m_ui->editPhone3->setText(m_query.value(18).toString());
  m_ui->editEpTakeover->setText(m_query.value(19).toString());
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
  QSqlQueryModel *model = new QSqlQueryModel(this);
  model->setQuery(m_query);

  m_ui->labelNr->setText(QString::number(number));
  m_ui->editPos->setText(QString::fromStdString(lastPos));
  m_ui->labelGenRate->setText(QString::number(m_hourlyRate));
  // TBD
  //m_ui->labelProfitTotalPerc->setText(QString::number(input.totalProfitMatPerc));
  //m_ui->labelProfitTotalEur->setText(QString::number(input.totalProfitMatEuro));

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
  connect(m_ui->editServiceCorr, &QLineEdit::textChanged, [this](QString txt)
  {
    data.corrFactor = txt.toDouble();
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
  double servicePriceCorr = servicePrice*data.corrFactor;
  m_ui->editServicePrice->setText(QString::number(servicePrice));
  m_ui->labelServiceQuant->setText(QString::number(servicePriceCorr));

  double ep = matPrice + servicePriceCorr + data.helpMat;
  m_ui->labelEP->setText(QString::number(ep));
  data.ep = ep;
  m_ui->labelPriceTotal->setText(QString::number(ep*data.number));
  data.total = ep*data.number;
}

void GeneralPage::TakeFromMaterial()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabNames::MaterialTab);
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
  auto tab = tabs.GetTabPointer(TabNames::ServiceTab);
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


InvoicePage::InvoicePage(Settings *settings, std::string const &invoiceNumber, QWidget *parent)
  : ParentPage("InvoicePage", parent)
  , m_ui(new Ui::invoicePage)
  , m_hourlyRate(settings->hourlyRate)
  , m_mwst(settings->mwst)
  , m_defaultHeading(settings->defaultHeading)
{
  m_ui->setupUi(this);

  m_ui->editInvoiceNumber->setText(QString::fromStdString(invoiceNumber));
  m_ui->editHourlyRate->setText(QString::number(m_hourlyRate));
  m_ui->editMwst->setText(QString::number(m_mwst));

  data = {};

  connect(m_ui->editInvoiceNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data.invoiceNumber = txt.toLongLong();
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    data.invoiceDate = txt;
  });
  connect(m_ui->editCustomerNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data.customerNumber = txt.toULong();
  });
  connect(m_ui->editSalutation, &QLineEdit::textChanged, [this](QString txt)
  {
    data.salutation = txt;
  });
  connect(m_ui->editName, &QLineEdit::textChanged, [this](QString txt)
  {
    data.name = txt;
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    data.street = txt;
  });
  connect(m_ui->editPlace, &QLineEdit::textChanged, [this](QString txt)
  {
    data.place = txt;
  });
  connect(m_ui->editSubject, &QLineEdit::textChanged, [this](QString txt)
  {
    data.subject = txt;
  });
  connect(m_ui->editMwst, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mwst = txt.toDouble();
  });
  connect(m_ui->editDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
  });
  connect(m_ui->editSkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
  });
  connect(m_ui->editDeliveryTime, &QLineEdit::textChanged, [this](QString txt)
  {
    data.deliveryDate = txt;
  });
  connect(m_ui->editHourlyRate, &QLineEdit::textChanged, [this](QString txt)
  {
    std::cout << "Edit hourly rate at invoice page not made yet\n";
  });
  connect(m_ui->editHeading, &QTextEdit::textChanged, [this]()
  {
    data.headline = m_ui->editHeading->toPlainText();;
  });
}

InvoicePage::~InvoicePage()
{}

void InvoicePage::TakeFromAdress()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabNames::AddressTab);
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

void InvoicePage::TakeDefaultHeading()
{
  m_ui->editHeading->setText(m_defaultHeading);
}


OfferPage::OfferPage(Settings *settings, std::string const &offerNumber, QWidget *parent)
  : ParentPage("OfferPage", parent)
  , m_ui(new Ui::offerPage)
  , m_hourlyRate(settings->hourlyRate)
  , m_defaultHeading(settings->defaultHeading)
{
  m_ui->setupUi(this);

  m_ui->editOfferNumber->setText(QString::fromStdString(offerNumber));
  m_ui->editHourlyRate->setText(QString::number(m_hourlyRate));

  data = {};

  connect(m_ui->editOfferNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data.offerNumber = txt.toLongLong();
  });
  connect(m_ui->editDate, &QLineEdit::textChanged, [this](QString txt)
  {
    data.offerDate = txt;
  });
  connect(m_ui->editCustomerNumber, &QLineEdit::textChanged, [this](QString txt)
  {
    data.customerNumber = txt.toULong();
  });
  connect(m_ui->editSalutation, &QLineEdit::textChanged, [this](QString txt)
  {
    data.salutation = txt;
  });
  connect(m_ui->editName, &QLineEdit::textChanged, [this](QString txt)
  {
    data.name = txt;
  });
  connect(m_ui->editStreet, &QLineEdit::textChanged, [this](QString txt)
  {
    data.street = txt;
  });
  connect(m_ui->editPlace, &QLineEdit::textChanged, [this](QString txt)
  {
    data.place = txt;
  });
  connect(m_ui->editSubject, &QLineEdit::textChanged, [this](QString txt)
  {
    data.subject = txt;
  });
  connect(m_ui->editDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
  });
  connect(m_ui->editSkonto, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
  });
  connect(m_ui->editHourlyRate, &QLineEdit::textChanged, [this](QString txt)
  {
    std::cout << "Edit hourly rate at invoice page not made yet\n";
  });
  connect(m_ui->editHeading, &QTextEdit::textChanged, [this]()
  {
    data.headline = m_ui->editHeading->toPlainText();;
  });
}

OfferPage::~OfferPage()
{}

void OfferPage::TakeFromAdress()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabNames::AddressTab);
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

void OfferPage::TakeDefaultHeading()
{
  m_ui->editHeading->setText(m_defaultHeading);
}