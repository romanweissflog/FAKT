/**
* @file adding_pages.h
*/

#ifndef ADDING_PAGES_H
#define ADDING_PAGES_H

#include "data_entries.h"
#include "sql_helper.hpp"
#include "log.h"
#include "overwatch.h"

#include "QtWidgets\qwidget.h"
#include "QtWidgets\qdialog.h"
#include "QtSql\qsqlquery.h"
#include "QtGui\qevent.h"

#include <memory>

namespace Ui
{
  class servicePage;
  class materialPage;
  class addressPage;
  class generalPage;
  class generalMainPage;
}


/**
* @class Parent class for all opened gui windows
*/
class ParentPage : public QDialog
{
public:
  /**
  * @brief Public constructor
  * @param childType Type of the inherited child
  * @param parent Parent widget
  */
  ParentPage(std::string const &childType, QWidget *parent = nullptr)
    : QDialog(parent)
    , m_logId(Log::GetLog().RegisterInstance(childType))
  {}

  /**
  * @brief Overwriten key press event
  */
  virtual void keyPressEvent(QKeyEvent *ev) override
  {
    if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
      return;
    QDialog::keyPressEvent(ev);
  }

protected:
  size_t m_logId;
};

/**
* @class Page for describing all listed services
*/
class ServicePage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param query Query to corresponding database
  * @param parent The parent object
  */
  ServicePage(Settings *settings, QSqlQuery &query, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~ServicePage();

private:
  /**
  * @brief To be clarified
  */
  void Calculate();

public slots:
  /**
  * @brief To be clarified
  */
  void CopyData(QString);

public:
  ServiceData data;         ///< internal data

private:
  Ui::servicePage *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
  double m_euroPerMin;      ///< convertion to price
};

/**
* @class Page for describing all listed materials
*/
class MaterialPage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param query Query to corresponding database
  * @param parent The parent object
  */
  MaterialPage(Settings *settings, QSqlQuery &query, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~MaterialPage();

private:
  /**
  * @brief To be clarified
  */
  void Calculate();

public slots:
  /**
  * @brief To be clarified
  */
  void CopyData(QString);

public:
  MaterialData data;        ///< internal data

private:
  Ui::materialPage *m_ui;   ///< gui element
  QSqlQuery &m_query;       ///< database query
  double m_mwst;            ///< mwst used on every entry
};

/**
* @class Page for describing all listed addresses
*/
class AddressPage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param query Query to corresponding database
  * @param edit ???
  * @param parent The parent object
  */
  AddressPage(Settings *settings, QSqlQuery &query, 
    QString edit = "", QWidget *parent = nullptr);
  
  /**
  * @brief Public destructor
  */
  ~AddressPage();

public slots:
  /**
  * @brief To be clarified
  */
  void CopyData(QString);

public:
  AddressData data;         ///< internal data

private:
  Ui::addressPage *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
};

/**
* @class Page for describing a single entry inside an offering or an invoice
*/
class GeneralPage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor for addind entry
  * @param number Internal used number for invoice / offering
  * @param lastPos Last position in this invoice / offering
  * @param query Query to corresponding database
  * @param parent The parent object
  */
  GeneralPage(Settings *settings,
    uint64_t number,
    std::string const &lastPos,
    QSqlQuery &query, 
    QWidget *parent = nullptr);

  void CopyData(uint64_t number, std::string const &pos);
  
  /**
  * @brief Public destructor
  */
  ~GeneralPage();

public slots:
  /**
  * @brief To be clarified
  */
  void TakeFromMaterial();

  /**
  * @brief To be clarified
  */
  void TakeFromService();

  /**
  * @brief To be clarified
  */
  void MakeNewEntry();

private:
  /**
  * @brief To be clarified
  */
  void Calculate();

  /**
  * @brief  Set connections
  */
  void SetConnections();

public:
  GeneralData data;       /// internal data

private:
  Ui::generalPage *m_ui;  ///< gui element
  QSqlQuery &m_query;     ///< database query
  double m_hourlyRate;    ///< hourly rate used for this piece
};

/**
* @class Page for describing the base of an offer, invoice or jobsite page
*/
class GeneralMainPage : public ParentPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  GeneralMainPage(Settings *settings, std::string const &number, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~GeneralMainPage();

  public slots:
  /**
  * @brief To be clarified
  */
  void TakeFromAdress();

  /**
  * @brief To be clarified
  */
  void TakeDefaultHeading();

protected:
  std::unique_ptr<GeneralMainData> internalData; ///< internal data

protected:
  Ui::generalMainPage *m_ui;  ///< gui element
  double m_hourlyRate;        ///< hourly rate for this invoice
  QString m_defaultHeading;   ///< The default heading as defined in settings
};


/**
* @class Page for describing a single invoice
*/
class InvoicePage : public GeneralMainPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  InvoicePage(Settings *settings, std::string const &invoiceNumber, TabName const &tab, QWidget *parent = nullptr);
  
  /**
  * @brief Public destructor
  */
  ~InvoicePage();

public:
  std::unique_ptr<InvoiceData> data;  ///< internal data
};


/**
* @class Page for describing a single offer
*/
class OfferPage : public GeneralMainPage
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  OfferPage(Settings *settings, std::string const &offerNumber, QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~OfferPage();

public:
  std::unique_ptr<OfferData> data;         ///< internal data
};

#endif