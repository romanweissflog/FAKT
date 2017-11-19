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
  PrintTypeInvoice,
  PrintTypeSingleOffer,
  PrintTypeSingleInvoice
};

class Export
{
public:
  Export(PrintType const &type = PrintTypeUndef);
  virtual ~Export() = default;
  std::string operator()(QSqlQuery const &query);

private:
  void PrintTitle();
  void PrintHeader();
  void PrintData(QSqlQuery const &query);
  void PrintEnding(QString const &ending);

private:
  int m_rc;
  PrintType m_type;
  std::string m_title;
  std::string m_header;
  std::string m_data;
  std::string m_ending;
};

#endif
