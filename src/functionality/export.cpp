#include "functionality\export.h"
#include "pages\general_print_page.h"
#include "functionality\log.h"
#include "functionality\overwatch.h"

#include "QtSql\qsqlquerymodel.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qfiledialog.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace
{
  QString GetTemplate(uint16_t const mask)
  {
    QString path = "templates/";

    //// pdf or print
    //if (mask & printmask::Pdf)
    //{
    //  path += "pdf/";
    //}
    //else if (mask & printmask::Print)
    //{
    //  path += "print/";
    //}
    //else
    //{
    //  throw std::runtime_error("Bad mask for export");
    //}

    // short or long
    if (mask & printmask::Short)
    {
      path += "short/";
    }
    else if (mask & printmask::Long)
    {
      path += "long/";
    }
    else
    {
      throw std::runtime_error("Bad short/long mask for export");
    }

    // type
    if (mask & printmask::Offer)
    {
      path += "offer/";
    }
    else if (mask & printmask::Invoice)
    {
      path += "invoice/";
    }
    else if (mask & printmask::Jobsite)
    {
      path += "jobsite/";
    }
    else
    {
      throw std::runtime_error("Bad type mask for export");
    }

    // what
    if (mask & printmask::Position)
    {
      path += "positions.lrxml";
    }
    else if (mask & printmask::Groups)
    {
      path += "groups.lrxml";
    }
    else if (mask & printmask::All)
    {
      path += "all.lrxml";
    }
    else
    {
      throw std::runtime_error("Bad what type mask for export");
    }
    return path;
  }
}

Export::Export(uint16_t mask, QWidget *parent)
  : m_report(new LimeReport::ReportEngine(this))
  , m_mask(mask)
  , QWidget(parent)
  , m_logId(Log::GetLog().RegisterInstance("Export"))
{
}

ReturnValue Export::operator()(TabName const &parentTab,
  QSqlQuery const &mainQuery, 
  QSqlQuery const &dataQuery,
  QSqlQuery const &groupQuery,
  QSqlQuery const &extraQuery,
  uint16_t withLogo)
{
  try
  {
    m_mask = 0;
    m_mask |= withLogo;
    uint16_t reportType;
    switch (parentTab)
    {
    case TabName::InvoiceTab: reportType = printmask::Invoice; break;
    case TabName::OfferTab: reportType = printmask::Offer; break;
    case TabName::JobsiteTab: reportType = printmask::Jobsite; break;
    default: throw std::runtime_error("Unsupported print page");
    }
    m_mask = m_mask | reportType;

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

    QSqlQueryModel *groupModel = new QSqlQueryModel(this);
    groupModel->setQuery(groupQuery);
    m_report->dataManager()->addModel("groups", groupModel, true);

    QSqlQueryModel *extraModel = new QSqlQueryModel(this);
    extraModel->setQuery(extraQuery);
    m_report->dataManager()->addModel("extra_info", extraModel, true);

    uint16_t subMask;
    GeneralPrintPage *page = new GeneralPrintPage(parentTab, mainData, subMask, this);
    connect(page, &GeneralPrintPage::Close, [this]()
    {
      emit Close();
    });
    emit Created(page);
    if (page->exec() == QDialog::Accepted)
    {
      m_mask |= subMask;
      try
      {
        auto const file = GetTemplate(m_mask);
        if (m_mask & printmask::Print)
        {
          if (!m_report->loadFromFile(file))
          {
            Log::GetLog().Write(LogType::LogTypeError, m_logId, m_report->lastError().toStdString());
            emit Close();
            return ReturnValue::ReturnAbort;
          }
          if (!m_report->printReport())
          {
            Log::GetLog().Write(LogType::LogTypeInfo, m_logId, m_report->lastError().toStdString());
            emit Close();
            return ReturnValue::ReturnAbort;
          }
        }
        else
        {
          QSqlQuery imageQuery(*Overwatch::GetInstance().GetDatabase());
          auto rc = imageQuery.exec("SELECT IMAGE FROM IMAGE_DATA WHERE TYPE = 'logo'");
          if (!rc)
          {
            Log::GetLog().Write(LogType::LogTypeError, m_logId, imageQuery.lastError().text().toStdString());
          }
          QSqlQueryModel *imageModel = new QSqlQueryModel(this);
          imageModel->setQuery(imageQuery);
          m_report->dataManager()->addModel("logo", imageModel, true);

          QString const fileName = QFileDialog::getSaveFileName(this,
            tr("Save Pdf"), "",
            tr("pdf file (*.pdf)"));
          if (fileName.size() == 0)
          {
            emit Close();
            return ReturnValue::ReturnAbort;
          }
          if (!m_report->loadFromFile(file))
          {
            Log::GetLog().Write(LogType::LogTypeError, m_logId, m_report->lastError().toStdString());
            emit Close();
            return ReturnValue::ReturnAbort;
          }
          if (!m_report->printToPDF(fileName))
          {
            Log::GetLog().Write(LogType::LogTypeError, m_logId, m_report->lastError().toStdString());
            emit Close();
            return ReturnValue::ReturnAbort;
          }
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
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
  catch (...)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Unknown failure");
  }
  emit Close();
  return ReturnValue::ReturnAbort;
}
