#include "tabs\jobsite.h"

#include "ui_basetab.h"

namespace
{
  TabData tabData
  {
    TabName::JobsiteTab,
    "Jobsite",
    "BAUSTELLE",
    "BA",
    "jobsites.db",
    "Baustellen",
    "RENR",
    printmask::Jobsite,
    {{
      { "RENR", { "Baust.Nr." } },
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
    }},
    { "RENR", "REDAT", "KUNR", "NAME", "GESAMT", "BRUTTO" }
  };
}


Jobsite::Jobsite(QWidget *parent)
  : GeneralTab(tabData, parent)
{
  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}
