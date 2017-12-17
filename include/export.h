#ifndef EXPORT_H_
#define EXPORT_H_

#include "QtSql\qsqlquerymodel.h"
#include "QtCore\qstring.h"

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

class Export
{
public:
  Export(PrintType const &type = PrintTypeUndef);
  virtual ~Export() = default;
  std::string operator()(QSqlQuery const &query);

  static void Prepare();

private:
  void PrintTitle();
  void PrintHeader();
  void PrintTableCols();
  void PrintData(QSqlQuery const &query);
  void PrintEnding(QString const &ending);

private:
  int m_rc;
  PrintType m_type;
  std::string m_title;
  std::string m_header;
  std::string m_tableCols;
  std::string m_data;
  std::string m_ending;
};

#endif
