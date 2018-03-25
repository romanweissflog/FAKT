#include "pages\general_print_page.h"
#include "functionality\log.h"

#include "ui_general_print_page.h"

#include "QtWidgets\qpushbutton.h"
#include "QtWidgets\qshortcut.h"

GeneralPrintPage::GeneralPrintPage(TabName const &parentTab, GeneralMainData const &data, uint8_t &subType, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::generalPrintPage)
  , m_logId(Log::GetLog().RegisterInstance("GeneralPrintPage"))
  , m_chosenSubType(subType)
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

  QPushButton *delivery = new QPushButton("Lieferschein (1)", this);
  connect(delivery, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::DeliveryNote; accept(); });
  m_ui->printLayout->addWidget(delivery);

  txt = "Aufma" + german::ss + "liste (leer) (6)";
  QPushButton *measureListEmpty = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(measureListEmpty, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::MeasureListEmpty; accept(); });
  m_ui->printLayout->addWidget(measureListEmpty);
  measureListEmpty->setEnabled(false);

  txt = "Aufma" + german::ss + "liste (7)";
  QPushButton *measureList = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(measureList, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::MeasureList; accept(); });
  m_ui->printLayout->addWidget(measureList);
  measureList->setEnabled(false);

  switch (parentTab)
  {
  case TabName::InvoiceTab: SetInvoiceData(); break;
  case TabName::JobsiteTab: SetJobsiteData(); break;
  case TabName::OfferTab: SetOfferData(); break;
  default:
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "invalid tab for printing");
    return;
  }

  connect(new QShortcut(QKeySequence(Qt::Key_1), this), &QShortcut::activated, [this]()
  {
    m_chosenSubType = PrintSubType::DeliveryNote;
    accept();
  });
  //connect(new QShortcut(QKeySequence(Qt::Key_6), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeJobsite::MeasureListEmpty;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_7), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubType::MeasureList;
  //  accept();
  //});
}

void GeneralPrintPage::keyPressEvent(QKeyEvent *e)
{
  if (e->key() != Qt::Key_Escape)
  {
    QDialog::keyPressEvent(e);
  }
  else
  {
    emit Close();
  }
}

void GeneralPrintPage::SetInvoiceData()
{
  m_ui->typeNumber->setText("Rechnungs-Nummer:");
  m_ui->typeDate->setText("Rechnungs-Datum:");

  QPushButton *type = new QPushButton("Rechnung (0)", this);
  connect(type, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeList = new QPushButton("Zeit-Liste (2)", this);
  connect(timeList, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeInvoice::TimeList; accept(); });
  m_ui->printLayout->insertWidget(2, timeList);
  timeList->setEnabled(false);

  connect(new QShortcut(QKeySequence(Qt::Key_0), this), &QShortcut::activated, [this]() {m_chosenSubType = PrintSubType::Type; accept(); });
  //connect(new QShortcut(QKeySequence(Qt::Key_2), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeInvoice::TimeList;
  //  accept();
  //});
}

void GeneralPrintPage::SetJobsiteData()
{
  m_ui->typeNumber->setText("Baustellen-Nummer:");
  m_ui->typeDate->setText("Baustellen-Datum:");

  QPushButton *type = new QPushButton("Baustelle (0)", this);
  connect(type, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeList = new QPushButton("Zeit-Liste (2)", this);
  connect(timeList, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeJobsite::TimeList; accept(); });
  m_ui->printLayout->insertWidget(2, timeList);
  timeList->setEnabled(false);

  std::string txt = "Auftragsbest" + german::ae + "tigung (3)";
  QPushButton *confirmation = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(confirmation, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeJobsite::Confirmation; accept(); });
  m_ui->printLayout->insertWidget(3, confirmation);
  confirmation->setEnabled(false);

  QPushButton *accounting = new QPushButton("Leistungsabrechnung (4)", this);
  connect(accounting, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeJobsite::Accounting; accept(); });
  m_ui->printLayout->insertWidget(4, accounting);
  accounting->setEnabled(false);

  connect(new QShortcut(QKeySequence(Qt::Key_0), this), &QShortcut::activated, [this]()
  {
    m_chosenSubType = PrintSubType::Type;
    accept();
  });
  //connect(new QShortcut(QKeySequence(Qt::Key_2), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeJobsite::TimeList;
  //  accept();
  //}); 
  //connect(new QShortcut(QKeySequence(Qt::Key_3), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeJobsite::Confirmation;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_4), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeJobsite::Accounting;
  //  accept();
  //});
}

void GeneralPrintPage::SetOfferData()
{
  m_ui->typeNumber->setText("Angebots-Nummer:");
  m_ui->typeDate->setText("Angebots-Datum:");

  QPushButton *type = new QPushButton("Angebot (0)", this);
  connect(type, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubType::Type; accept(); });
  m_ui->printLayout->insertWidget(0, type);

  QPushButton *timeListShort = new QPushButton("Zeit-Liste kurz (2)", this);
  connect(timeListShort, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeOffer::TimeListShort; accept(); });
  m_ui->printLayout->insertWidget(2, timeListShort);
  timeListShort->setEnabled(false);

  QPushButton *timeListLong = new QPushButton("Zeit-Liste lang (3)", this);
  connect(timeListLong, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeOffer::TimeListLong; accept(); });
  m_ui->printLayout->insertWidget(3, timeListLong);
  timeListLong->setEnabled(false);

  QPushButton *order = new QPushButton("Auftrag (4)", this);
  connect(order, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeOffer::Order; accept(); });
  m_ui->printLayout->insertWidget(4, order);
  order->setEnabled(false);

  std::string txt = "Auftragsbest" + german::ae + "tigung (5)";
  QPushButton *confirmation = new QPushButton(QString::fromUtf8(txt.c_str()), this);
  connect(confirmation, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeOffer::Confirmation; accept(); });
  m_ui->printLayout->insertWidget(5, confirmation);
  confirmation->setEnabled(false);

  QPushButton *inquiry = new QPushButton("Preisanfrage (8)", this);
  connect(inquiry, &QPushButton::clicked, [this]() {m_chosenSubType = PrintSubTypeOffer::Inquiry; accept(); });
  m_ui->printLayout->addWidget(inquiry);
  inquiry->setEnabled(false);

  connect(new QShortcut(QKeySequence(Qt::Key_0), this), &QShortcut::activated, [this]() { m_chosenSubType = PrintSubType::Type; accept(); });
  //connect(new QShortcut(QKeySequence(Qt::Key_2), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeOffer::TimeListShort;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_3), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeOffer::TimeListShort;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_4), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeOffer::Order;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_5), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeOffer::Confirmation;
  //  accept();
  //});
  //connect(new QShortcut(QKeySequence(Qt::Key_8), this), &QShortcut::activated, [this]()
  //{
  //  chosenSubType = PrintSubTypeOffer::Inquiry;
  //  accept();
  //});
}
