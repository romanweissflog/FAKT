#include "tabs\material.h"

#include "ui_basetab.h"

namespace
{
  TabData tabData
  {
    TabName::MaterialTab,
    "Material",
    "MATERIAL",
    "",
    "",
    "Material",
    "ARTNR",
    printmask::Undef,
    {{
      { "ARTNR", { "Schl.-Nr." } },
      { "HAUPTARTBEZ", { "Bezeichnung" } },
      { "ARTBEZ", { "Extra-Information" } },
      { "ME", { "Einheit" } },
      { "NETTO", { "Netto" } },
      { "BRUTTO", { "Brutto" } },
      { "EKP", { "EKP" } },
      { "BAUZEIT", { "Minuten" } }
    }},
    { "ARTNR", "HAUPTARTBEZ", "ME", "NETTO", "BRUTTO", "EKP", "VERARB", "BAUZEIT" }
  };
}


Material::Material(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}
