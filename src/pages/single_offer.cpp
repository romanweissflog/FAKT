#include "pages\single_offer.h"
#include "functionality\overwatch.h"
#include "pages\offer_page.h"


SingleOffer::SingleOffer(size_t number, std::string const &tableName, QWidget *parent)
  : SingleEntry(number, tableName, TabName::OfferTab, parent)
  , data(static_cast<OfferData*>(m_internalData))
{
  this->setWindowTitle("Angebot");
}

void SingleOffer::Calculate()
{
  data->total = data->materialTotal + data->helperTotal + data->serviceTotal;
  data->mwstTotal = data->total / 100 * m_settings->mwst;
  data->brutto = data->total + data->mwstTotal;
  data->skonto = data->brutto / 100 * data->skonto + data->brutto;
}

void SingleOffer::EditMeta()
{
  std::string number = std::to_string(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::OfferTab);
  OfferPage *editPage = new OfferPage(m_settings, number);
  OfferData *data = static_cast<OfferData*>(tab->GetData(number));
  editPage->SetData(data);
  if (editPage->exec() == QDialog::Accepted)
  {
    tab->SetData(editPage->data);
  }
}