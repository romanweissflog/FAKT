#ifndef SERVICE_PAGE_H
#define SERVICE_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class servicePage;
}

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
  ServicePage(Settings *settings,
    QSqlQuery &query,
    QString const &edit = "",
    QWidget *parent = nullptr);

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

#endif
