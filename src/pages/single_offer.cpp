#include "pages\single_offer.h"
#include "functionality\overwatch.h"
#include "pages\offer_page.h"


SingleOffer::SingleOffer(size_t number, QWidget *parent)
  : SingleEntry(number, "A", TabName::OfferTab, parent)
{
  m_data.tabName = "Angebote";
  this->setWindowTitle("Angebot");
}

void SingleOffer::Calculate()
{
  try
  {
    double const materialTotal = data.GetDouble("MGESAMT");
    double const serviceTotal = data.GetDouble("LGESAMT");
    double const helperTotal = data.GetDouble("SGESAMT");
    double const total = materialTotal + serviceTotal + helperTotal;
    double const mwst = m_settings->mwst;
    double const mwstTotal = total / 100 * mwst;
    double const brutto = total + mwstTotal;
    data["GESAMT"].entry = total;
    data["MWSTGESAMT"].entry = mwstTotal;
    data["BRUTTO"].entry = brutto;
  }
  CATCHANDLOGERROR
}

void SingleOffer::Recalculate(DatabaseData const &edited)
{
  try
  {
    double const total = data.GetDouble("GESAMT");
    double const mwst = m_settings->mwst;
    double const mwstTotal = total / 100 * mwst;
    double const brutto = total + mwstTotal;
    data["MWSTGESAMT"].entry = mwstTotal;
    data["BRUTTO"].entry = brutto;
    SingleEntry::Recalculate(edited);
  }
  CATCHANDLOGERROR
}
