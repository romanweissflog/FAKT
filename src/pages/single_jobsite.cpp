#include "pages\single_jobsite.h"
#include "pages\invoice_page.h"
#include "functionality\overwatch.h"


SingleJobsite::SingleJobsite(size_t number, QWidget *parent)
  : SingleEntry(number, "BA", TabName::JobsiteTab, parent)
{
  m_data.tabName = "Baustellen";
  this->setWindowTitle("Baustelle");
}

void SingleJobsite::Calculate()
{
  try
  {
    double const materialTotal = data.GetDouble("MGESAMT");
    double const serviceTotal = data.GetDouble("LGESAMT");
    double const helperTotal = data.GetDouble("SGESAMT");
    double const total = materialTotal + serviceTotal + helperTotal;
    double const mwst = data.GetDouble("MWSTSATZ");
    double const mwstTotal = total / 100 * mwst;
    double const brutto = total + mwstTotal;
    double const skonto = data.GetDouble("SKONTO");
    data["GESAMT"].entry = total;
    data["MWSTGESAMT"].entry = mwstTotal;
    data["BRUTTO"].entry = brutto;
    data["SKBETRAG"].entry = brutto - brutto / 100 * skonto;
  }
  CATCHANDLOGERROR
}

void SingleJobsite::Recalculate(DatabaseData const &edited)
{
  try
  {
    double const total = data.GetDouble("GESAMT");
    double const mwst = edited.GetDouble("MWSTSATZ");
    double const mwstTotal = total / 100 * mwst;
    double const brutto = total + mwstTotal;
    double const skonto = edited.GetDouble("SKONTO");
    data["MWSTGESAMT"].entry = mwstTotal;
    data["BRUTTO"].entry = brutto;
    data["SKBETRAG"].entry = brutto - brutto / 100 * skonto;
    SingleEntry::Recalculate(edited);
  }
  CATCHANDLOGERROR
}
