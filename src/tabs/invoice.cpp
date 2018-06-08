#include "tabs\invoice.h"
#include "tabs\payment.h"

#include "ui_basetab.h"

#include "QtWidgets\qshortcut.h"

namespace
{
  TabData tabData
  {
    TabName::InvoiceTab,
    "Invoice",
    "RECHNUNG",
    "R",
    "invoices.db",
    "Rechnungen",
    "RENR",
    printmask::Invoice,
    {
      { "RENR", { "Rechnungs-Nr." } },
      { "REDAT", { "Datum" } },
      { "KUNR", { "K.-Nr." } },
      { "NAME", { "Name" } },
      { "GESAMT", { "Netto" } },
      { "BRUTTO", { "Brutto" } },
      { "ANREDE", { "Anrede" } },
      { "STRASSE", { QString::fromStdString("Stra" + german::ss + "e") } },
      { "ORT", { "Ort" } },
      { "MGESAMT", { "Material" } },
      { "LGESAMT", { "Leistung" } },
      { "SGESAMT", { "S-Zeug" } },
      { "MWSTGESAMT", { "MwstGesamt" } },
      { "SKONTO", { "Skonto" } },
      { "SKBETRAG", { "Skonto-Betrag" } },
      { "BEZAHLT", { "Bezahlt" } },
      { "HEADLIN", { "Header" } },
      { "BEZADAT", { "Bezahldatum" } },
      { "LIEFDAT", { "Lieferdatum" } },
      { "RABATT", { "Rabatt" } },
      { "Z_FRIST_N", { "Zahlung normal" } },
      { "Z_FRIST_S", { "Zahlung Skonto" } },
      { "SCHLUSS", { "Schluss" } },
      { "STUSATZ", { "Stundensatz" } },
      { "BETREFF", { "Betreff" } },
      { "MWSTSATZ", { "Mwst" } }
    },
    { "RENR", "REDAT", "KUNR", "NAME", "GESAMT", "BRUTTO" }
  };
}


Invoice::Invoice(QWidget *parent)
  : GeneralTab(tabData, parent)
{
  QPushButton *payment = new QPushButton("Zahlungseingang (Z)", this);
  connect(payment, &QPushButton::clicked, this, &Invoice::OpenPayment);
  m_ui->layoutAction->addWidget(payment);
  new QShortcut(QKeySequence(Qt::Key_Z), this, SLOT(OpenPayment()));
}

void Invoice::OpenPayment()
{
  Payment *payment = new Payment(m_query, this); 
  emit AddSubtab(payment, "Rechnungen:Zahlungen");
  payment->ShowDatabase();
}
