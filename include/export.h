#ifndef EXPORT_H_
#define EXPORT_H_

#include "QtSql\qsqlquery.h"
#include "QtCore\qstring.h"
#include "QtGui\qtextdocument.h"

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
  PrintTypeSingleInvoice
};


struct PrintData
{
  QString what;
  QString salutation;
  QString name;
  QString street;
  QString place;
  QString number;
  QString date;
  double mwst;
  double netto;
  double mwstPrice;
  double brutto;
  QString headline;
  QString subject;
  QString endline;
};


class Export
{
public:
  Export(PrintType const &type = PrintTypeUndef);
  virtual ~Export() = default;
  void operator()(QTextCursor &cursor, PrintData const &data, QSqlQuery &dataQuery, std::string const &logo = "");
  
private:
  void PrintHeader(QTextCursor &cursor, PrintData const &data);
  void PrintQuery(QTextCursor &cursor, QSqlQuery &query);
  void PrintResult(QTextCursor &cursor, PrintData const &data);
  void PrintEnding(QTextCursor &cursor, PrintData const &data);

private:
  int m_rc;
  PrintType m_type;
};

#endif
