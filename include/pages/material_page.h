#ifndef MATERIAL_PAGE_H
#define MATERIAL_PAGE_H

#include "parent_page.h"

#include "QtSql\qsqlquery.h"

namespace Ui
{
  class materialPage;
}

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
  MaterialPage(Settings *settings, QSqlQuery &query,
    QString const &edit = "", QWidget *parent = nullptr);

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

#endif
