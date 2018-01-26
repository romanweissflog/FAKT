#ifndef ADDRESS_PAGE_H
#define ADDRESS_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class addressPage;
}

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
  AddressPage(Settings *settings, QSqlQuery &query, std::string const &number,
    QString const &edit = "", QWidget *parent = nullptr);

  /**
  * @brief Public destructor
  */
  ~AddressPage();

  public slots:
  void CopyData(QString);

public:
  AddressData data;         ///< internal data

private:
  Ui::addressPage *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
};


#endif
