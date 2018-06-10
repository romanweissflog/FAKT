#include "pages\single_invoice.h"
#include "functionality\overwatch.h"
#include "pages\invoice_page.h"


SingleInvoice::SingleInvoice(size_t number, QWidget *parent)
  : SingleEntry(number, "R", TabName::InvoiceTab, parent)
{
  m_data.tabName = "Rechnungen";
  this->setWindowTitle("Rechnung");
}

void SingleInvoice::Calculate()
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

void SingleInvoice::Recalculate(DatabaseData const &edited)
{
  double const total = edited.GetDouble("GESAMT");
  double const mwst = edited.GetDouble("MWSTSATZ");
  double const mwstTotal = total / 100 * mwst;
  double const brutto = total + mwstTotal;
  double const skonto = edited.GetDouble("SKONTO");
  data["MWSTGESAMT"].entry = mwstTotal;
  data["BRUTTO"].entry = brutto;
  data["SKBETRAG"].entry = brutto - brutto / 100 * skonto;
  SingleEntry::Recalculate(edited);
}

void SingleInvoice::EditMeta()
{
  QString number = QString::number(m_number);
  auto tab = Overwatch::GetInstance().GetTabPointer(TabName::InvoiceTab);
  InvoicePage *editPage = new InvoicePage(m_settings, number, TabName::InvoiceTab);
  connect(editPage, &PageFramework::AddExtraPage, [this, editPage, number](QWidget *widget, QString const &txt)
  {
    emit AddSubtab(widget, "Angebote:" + number + ":Allgemein:" + txt);
  });
  connect(editPage, &PageFramework::CloseExtraPage, [this, editPage, number](QString const &txt)
  {
    emit CloseTab("Angebote:" + number + ":Allgemein:" + txt);
  });

  DatabaseData const data = tab->GetData(number.toStdString());
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
