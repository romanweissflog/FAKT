/**
* @file adding_pages.h
*/

#ifndef ADDING_PAGES_H
#define ADDING_PAGES_H

#include "data_entries.h"
#include "sql_helper.hpp"

#include "QtWidgets\qwidget.h"
#include "QtWidgets\qdialog.h"
#include "QtSql\qsqlquery.h"

namespace Ui
{
  class servicePage;
  class materialPage;
  class addressPage;
  class generalPage;
  class invoicePage;
}

/**
* @class Page for describing all listed services
*/
class ServicePage : public QDialog
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

  /**
  * @brief Overwriten key press event
  */
  void keyPressEvent(QKeyEvent *ev) override;

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
class MaterialPage : public QDialog
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

  /**
  * @brief Overwriten key press event
  */
  void keyPressEvent(QKeyEvent *ev) override;

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
class AddressPage : public QDialog
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
  
  /**
  * @brief Overwriten key press event
  */
  void keyPressEvent(QKeyEvent *ev) override;

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
class GeneralPage : public QDialog
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
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
  
  /**
  * @brief Public destructor
  */
  ~GeneralPage();
 
  /**
  * @brief Overwriten key press event
  */
  void keyPressEvent(QKeyEvent *ev) override;

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

public:
  GeneralData data;       /// internal data

private:
  Ui::generalPage *m_ui;  ///< gui element
  QSqlQuery &m_query;     ///< database query
  double m_hourlyRate;    ///< hourly rate used for this piece
};

/**
* @class Page for describing a single invoice
*/
class InvoicePage : public QDialog
{
  Q_OBJECT
public:
  /**
  * @brief Public constructor
  * @param settings Settings read by settings file
  * @param invoiceNumber The corresponding invoice number
  * @param parent The parent object
  */
  InvoicePage(Settings *settings, uint64_t invoiceNumber, QWidget *parent = nullptr);
  
  /**
  * @brief Public destructor
  */
  ~InvoicePage();
  
  /**
  * @brief Overwriten key press event
  */
  void keyPressEvent(QKeyEvent *ev) override;

public slots:
  /**
  * @brief To be clarified
  */
  void TakeFromAdress();

  /**
  * @brief To be clarified
  */
  void TakeDefaultHeading();

public:
  InvoiceData data;         ///< internal data

private:
  Ui::invoicePage *m_ui;    ///< gui element
  double m_hourlyRate;      ///< hourly rate for this invoice
  double m_mwst;            ///< mwst for this invoice
  QString m_defaultHeading; ///< The default heading as defined in settings
};

#endif