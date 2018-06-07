#include "tabs\service.h"

#include "ui_basetab.h"

namespace
{
  TabData tabData
  {
    TabName::ServiceTab,
    "Service",
    "LEISTUNG",
    "",
    "",
    "Leistungen",
    "ARTNR",
    printmask::Undef,
    {
      { "ARTNR", { "Schl.-Nr." } },
      { "HAUPTARTBEZ", { "Bezeichnung" } },
      { "ARTBEZ", { "Extra-Information" } },
      { "ME", { "Einheit" } },
      { "EP", { "EP" } },
      { "LP", { "Leistung" } },
      { "MP", { "Material" } },
      { "SP", { "Hilfsmat." } },
      { "BAUZEIT", { "Minuten" } },
      { "EKP", { "EKP" } }
    },
    { "ARTNR", "HAUPTARTBEZ", "ME", "EP", "LP", "MP", "SP", "BAUZEIT", "EKP" }
  };
}


Service::Service(QWidget *parent)
  : BaseTab(tabData, parent)
{
  m_ui->printEntry->setEnabled(false);
  m_ui->pdfExport->setEnabled(false);

  m_proxyModel->sort(0, Qt::SortOrder::AscendingOrder);
}
