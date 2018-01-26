#include "general_print_page.h"

#include "ui_general_print_page.h"

#include "QtWidgets\qpushbutton.h"

GeneralPrintPage::GeneralPrintPage(PrintData const &data, uint8_t &subType, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::generalPrintPage)
  , chosenSubType(subType)
{
  m_ui->setupUi(this);
  this->setAttribute(Qt::WA_DeleteOnClose);

  // prepare static gui data
  m_ui->date->setText(data.date);
  m_ui->number->setText(data.number);
  m_ui->salutation->setText(data.salutation);
  m_ui->name->setText(data.name);
  m_ui->street->setText(data.street);
  m_ui->place->setText(data.place);

  // set static gui buttons
  std::string txt;

  QPushButton *delivery = new QPushButton("Lieferschein", this);
  connect(delivery, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::DeliveryNote; accept(); });
  m_ui->printLayout->addWidget(delivery);

  txt = "Aufma" + german::ss + "liste (leer)";
  QPushButton *measureListEmpty = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(measureListEmpty, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::MeasureListEmpty; accept(); });
  m_ui->printLayout->addWidget(measureListEmpty);
  measureListEmpty->setEnabled(false);

  txt = "Aufma" + german::ss + "liste";
  QPushButton *measureList = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(measureList, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::MeasureList; accept(); });
  m_ui->printLayout->addWidget(measureList);
  measureList->setEnabled(false);

  switch (data.tab)
  {
  case TabName::InvoiceTab: SetInvoiceData(); break;
  case TabName::JobsiteTab: SetJobsiteData(); break;
  case TabName::OfferTab: SetOfferData(); break;
  default: throw std::runtime_error("Not supported print type");
  }

  show();
}

void GeneralPrintPage::SetInvoiceData()
{
  m_ui->typeNumber->setText("Rechnungs-Nummer:");
  m_ui->typeDate->setText("Rechnungs-Datum:");

  QPushButton *type = new QPushButton("Rechnung", this);
  connect(type, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeList = new QPushButton("Zeit-Liste", this);
  connect(timeList, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeInvoice::TimeList; accept(); });
  m_ui->printLayout->insertWidget(2, timeList);
  timeList->setEnabled(false);
}

void GeneralPrintPage::SetJobsiteData()
{
  m_ui->typeNumber->setText("Baustellen-Nummer:");
  m_ui->typeDate->setText("Baustellen-Datum:");

  QPushButton *type = new QPushButton("Baustelle", this);
  connect(type, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeList = new QPushButton("Zeit-Liste", this);
  connect(timeList, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeJobsite::TimeList; accept(); });
  m_ui->printLayout->insertWidget(2, timeList);
  timeList->setEnabled(false);

  std::string txt = "Auftragsbest" + german::ae + "tigung";
  QPushButton *confirmation = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(confirmation, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeJobsite::Confirmation; accept(); });
  m_ui->printLayout->insertWidget(3, confirmation);
  confirmation->setEnabled(false);

  QPushButton *accounting = new QPushButton("Leistungsabrechnung", this);
  connect(accounting, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeJobsite::Accounting; accept(); });
  m_ui->printLayout->insertWidget(4, accounting);
  accounting->setEnabled(false);
}

void GeneralPrintPage::SetOfferData()
{
  m_ui->typeNumber->setText("Angebots-Nummer:");
  m_ui->typeDate->setText("Angebots-Datum:");

  QPushButton *type = new QPushButton("Angebot", this);
  connect(type, &QPushButton::clicked, [this]() {chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeListShort = new QPushButton("Zeit-Liste kurz", this);
  connect(timeListShort, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeOffer::TimeListShort; accept(); });
  m_ui->printLayout->insertWidget(2, timeListShort);
  timeListShort->setEnabled(false);

  QPushButton *timeListLong = new QPushButton("Zeit-Liste lang", this);
  connect(timeListLong, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeOffer::TimeListLong; accept(); });
  m_ui->printLayout->insertWidget(3, timeListLong);
  timeListLong->setEnabled(false);

  QPushButton *order = new QPushButton("Auftrag", this);
  connect(order, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeOffer::Order; accept(); });
  m_ui->printLayout->insertWidget(4, order);
  order->setEnabled(false);

  std::string txt = "Auftragsbest" + german::ae + "tigung";
  QPushButton *confirmation = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(confirmation, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeOffer::Confirmation; accept(); });
  m_ui->printLayout->insertWidget(5, confirmation);
  confirmation->setEnabled(false);

  QPushButton *inquiry = new QPushButton("Preisanfrage", this);
  connect(inquiry, &QPushButton::clicked, [this]() {chosenSubType = PrintSubTypeOffer::Inquiry; accept(); });
  m_ui->printLayout->addWidget(inquiry);
  inquiry->setEnabled(false);
}
