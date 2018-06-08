#include "tabs\offer.h"

#include "ui_basetab.h"

namespace
{
  TabData tabData
  {
    TabName::OfferTab,
    "Offer",
    "ANGEBOT",
    "A",
    "offers.db",
    "Angebote",
    "RENR",
    printmask::Offer,
    {
      { "RENR", { "Ang-.Nr." } },
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
      { "SGESAMT", { "Sonder" } },
      { "MWSTGESAMT", { "MwstGesamt" } },
      { "HEADLIN", { "Header" } },
      { "RABATT", { "Rabatt" } },
      { "SCHLUSS", { "Schluss" } },
      { "STUSATZ", { "Stundensatz" } },
      { "BETREFF", { "Betreff" } },
      { "B_FRIST", { "Bindefrist" } },
      { "Z_FRIST_N", { "Zahlung normal" } },
      { "Z_FRIST_S", { "Zahlung Skonto" } },
      { "SKONTO", { "Skonto" } }
    },
    { "RENR", "REDAT", "KUNR", "NAME", "GESAMT", "BRUTTO" }
  };
}


Offer::Offer(QWidget *parent)
  : GeneralTab(tabData, parent)
{
  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}
