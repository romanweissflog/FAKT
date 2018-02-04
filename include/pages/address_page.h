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
  AddressPage(Settings *settings, QSqlQuery &query, QString const &number,
    QString const &edit = "", QWidget *parent = nullptr);

  ~AddressPage();
  void SetFocusToFirst() override;

public slots:
  void CopyData(QString);

public:
  AddressData data;         ///< internal data

private:
  Ui::addressPage *m_ui;    ///< gui element
  QSqlQuery &m_query;       ///< database query
};


#endif
