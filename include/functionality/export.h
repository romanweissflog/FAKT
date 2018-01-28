#ifndef EXPORT_H_
#define EXPORT_H_

#include "data_entries.h"
#include "defines.h"

#include "QtSql\qsqlquery.h"
#include "QtCore\qstring.h"
#include "QtGui\qtextdocument.h"
#include "QtCore\qobject.h"

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
  PrintTypeDeliveryNote
};


class Export : public QObject
{
  Q_OBJECT
public:
  Export(PrintType const &type = PrintTypeUndef);
  virtual ~Export() = default;
  ReturnValue operator()(QTextCursor &cursor, PrintData const &data, QSqlQuery &dataQuery, std::string const &logo = "");

signals:
  void Created(QWidget *page);
  void Close();
  
private:
  void PrintHeader(QTextCursor &cursor, uint8_t subType, PrintData const &data);
  void PrintQuery(QTextCursor &cursor, uint8_t subType, QSqlQuery &query);
  void PrintResult(QTextCursor &cursor, uint8_t subType, PrintData const &data);
  void PrintEnding(QTextCursor &cursor, uint8_t subType, PrintData const &data);

private:
  int m_rc;
  PrintType m_type;
};

#endif
