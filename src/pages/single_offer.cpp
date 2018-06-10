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

void SingleOffer::Recalculate(DatabaseData const &edited)
{
  double const total = edited.GetDouble("GESAMT");
  double const mwst = m_settings->mwst;
  double const mwstTotal = total / 100 * mwst;
  double const brutto = total + mwstTotal;
  data["MWSTGESAMT"].entry = mwstTotal;
  data["BRUTTO"].entry = brutto;
  SingleEntry::Recalculate(edited);
}

void SingleOffer::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::OfferTab);
  OfferPage *editPage = new OfferPage(m_settings, number);
  connect(editPage, &PageFramework::AddExtraPage, [this, editPage, number](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Angebote:" + number + ":Allgemein:" + txt);
  });
  connect(editPage, &PageFramework::CloseExtraPage, [this, editPage, number](QString const &txt)
  {
    emit CloseTab("Angebote:" + number + ":Allgemein:" + txt);
  });

  auto const data = tab->GetData(number.toStdString());
  editPage->content->SetData(data);
  editPage->content->LockNumber();
  
  QString const tabName = m_data.tabName + ":" + QString::number(m_number) + ":Allgemein";
  AddSubtab(editPage, tabName);
  connect(editPage, &PageFramework::Accepted, [this, tab, editPage, tabName]()
  {
    Recalculate(editPage->content->data);
    tab->SetData(editPage->content->data);
    emit CloseTab(tabName);
  });
  connect(editPage, &PageFramework::Declined, [this, tabName]()
  {
    emit CloseTab(tabName);
  });
}
