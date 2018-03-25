#include "functionality\export.h"
#include "pages\general_print_page.h"

#include "QtSql\qsqlquerymodel.h"
#include "QtWidgets\qfiledialog.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace
{
  QString GetTemplate(TabName const &tab, uint8_t subType)
  {
    QString file = "";
    switch (tab)
    {
    case TabName::OfferTab:
      if (subType == PrintSubType::Type)
      {
        file = "C:/Users/roman/Documents/Projekte/FAKT/templates/general_offer.lrxml";
      }
      break;
    case TabName::JobsiteTab:
      break;
    case TabName::InvoiceTab:
      break;
    default:
      throw std::runtime_error("Unsupported tab print type");
    }
    if (file.size() == 0)
    {
      throw std::runtime_error("Unsupported sub print type");
    }
    return file;
  }
}

Export::Export(PrintType const &type, QWidget *parent)
  : m_report(new LimeReport::ReportEngine(this))
  , m_type(type)
  , QWidget(parent)
{
}

ReturnValue Export::operator()(TabName const &parentTab,
  QSqlQuery const &mainQuery, 
  QSqlQuery const &dataQuery, 
  std::string const &logo)
{
  GeneralMainData mainData;
  mainData.salutation = mainQuery.value("ANREDE").toString();
  mainData.name = mainQuery.value("NAME").toString();
  mainData.street = mainQuery.value("STRASSE").toString();
  mainData.place = mainQuery.value("ORT").toString();
  mainData.number = mainQuery.value("RENR").toString();
  mainData.date = mainQuery.value("REDAT").toString();
  mainData.total = mainQuery.value("GESAMT").toDouble();
  mainData.mwstTotal = mainQuery.value("MWSTGESAMT").toDouble();
  mainData.brutto = mainQuery.value("BRUTTO").toDouble();
  mainData.headline = mainQuery.value("HEADLIN").toString();
  mainData.subject = mainQuery.value("BETREFF").toString();
  mainData.endline = mainQuery.value("SCHLUSS").toString();

  QSqlQueryModel *mainModel = new QSqlQueryModel(this);
  mainModel->setQuery(mainQuery);
  m_report->dataManager()->addModel("main_data", mainModel, true);

  QSqlQueryModel *dataModel = new QSqlQueryModel(this);
  dataModel->setQuery(dataQuery);
  m_report->dataManager()->addModel("positions", dataModel, true);
  
  uint8_t subType{};
  GeneralPrintPage *page = new GeneralPrintPage(parentTab, mainData, subType, this);
  connect(page, &GeneralPrintPage::Close, [this]()
  {
    emit Close();
  });
  emit Created(page);
  if (page->exec() == QDialog::Accepted)
  {
    try
    {
      m_report->loadFromFile(GetTemplate(parentTab, subType));
      if (logo.size() == 0)
      {
        m_report->printReport();
      }
      else
      {
        QString const fileName = QFileDialog::getSaveFileName(this,
          tr("Save Pdf"), "",
          tr("pdf file (*.pdf)"));
        if (fileName.size() == 0)
        {
          emit Close();
          return ReturnValue::ReturnAbort;
        }
        m_report->printToPDF(fileName);
      }
      emit Close();
      return ReturnValue::ReturnSuccess;
    }
    catch (...)
    {
      emit Close();
      return ReturnValue::ReturnFailure;
    }
  }
  emit Close();
  return ReturnValue::ReturnAbort;
}
