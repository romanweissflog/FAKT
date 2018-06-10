#include "tabs\address.h"
#include "pages\address_page.h"
#include "functionality\sql_helper.h"

#include "ui_basetab.h"

namespace
{
  TabData tabData
  {
    TabName::AddressTab,
    "Address",
    "ADRESSEN",
    "",
    "",
    "Adressen",
    "SUCHNAME",
    printmask::Undef,
    {{
      { "SUCHNAME", { "Suchname" } },
      { "TELEFON",{ "Telefon" } },
      { "KUNR", { "K.-Nummer" } },
      { "NAME", { "Name" } },
      { "PLZ", { "PLZ" } },
      { "ORT", { "Ort" } },
      { "STRASSE", { QString::fromStdString("Stra" + german::ss + "e") } },
      { "ANREDE", { "Anrede" } },
      { "FAX", { "Fax" } },
      { "EMAIL", { "E-mail" } }
    }},
    { "SUCHNAME", "TELEFON", "KUNR", "NAME", "PLZ", "ORT", "STRASSE" }
  };
}


Address::Address(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}
