#ifndef EXPORT_H_
#define EXPORT_H_

#include "data_entries.h"
#include "defines.h"

#include "lrreportengine.h"

#include "QtSql\qsqlquery.h"
#include "QtCore\qobject.h"
#include "QtPrintSupport\qprinter.h"

#include <cstdint>
#include <string>

enum PrintType : uint8_t
{
  PrintTypeUndef,
  PrintTypeAddress,
  PrintTypeMaterial,
  PrintTypeService,
  PrintTypeOffer,
  PrintTypeJobsite,
  PrintTypeInvoice,
  PrintTypeSingleOffer,
  PrintTypeSingleJobsite,
  PrintTypeSingleInvoice,
  PrintTypePayment
};


class Export : public QWidget
{
  Q_OBJECT
public:
  Export(PrintType const &type = PrintTypeUndef, QWidget *parent = nullptr);
  virtual ~Export() = default;
  ReturnValue operator()(TabName const &parentTab, 
    QSqlQuery const &mainQuery, 
    QSqlQuery const &dataQuery, 
    std::string const &logo = "");

signals:
  void Created(QWidget *page);
  void Close();

private:
  LimeReport::ReportEngine *m_report;
  int m_rc;
  PrintType m_type;
};

#endif
