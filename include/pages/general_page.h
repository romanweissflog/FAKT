#ifndef GENERAL_PAGE_H
#define GENERAL_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class generalPage;
}

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
    std::string const &child,
    QSqlQuery &query,
    QWidget *parent = nullptr);

  void CopyData(std::string const &table, std::string const &pos);

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

#endif
