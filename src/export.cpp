#include "include/export.h"

#include "QtSql\qsqlerror.h"

#include <map>
#include <vector>

namespace
{
  struct PrintData
  {
    std::string title;
    std::string heading;
    uint16_t endingPosition;
    std::vector<uint16_t> dataPositions;
  };
  std::map<PrintType, PrintData> printData;
}

Export::Export(PrintType const &type)
  : m_type(type)
{
  PrintTitle();
  PrintHeader();
}

std::string Export::operator()(QSqlQuery const &query)
{
  PrintData(query);

  return m_title + m_header + m_data + m_ending;
}

void Export::PrintTitle()
{
  m_title = printData[m_type].title;
}

void Export::PrintHeader()
{
  m_header = printData[m_type].heading;
}

void Export::PrintData(QSqlQuery const &query)
{

}

void Export::PrintEnding(QString const &ending)
{

}
