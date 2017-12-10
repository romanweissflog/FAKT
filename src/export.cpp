#include "include/export.h"

#include "QtSql\qsqlerror.h"

#include <map>
#include <vector>
#include <string>

namespace
{
  struct PrintData
  {
    std::string title;
    std::string tableCols;
    uint16_t endingPosition;
    std::vector<uint16_t> dataPositions;
  };
  std::map<PrintType, PrintData> printData;

  void PrepareAddress()
  {
    PrintData data;
    printData[PrintType::PrintTypeAddress] = data;
  }

  void PrepareMaterial()
  {
    PrintData data;
    printData[PrintType::PrintTypeMaterial] = data;
  }

  void PrepareService()
  {
    PrintData data;
    printData[PrintType::PrintTypeService] = data;
  }

  void PrepareOffer()
  {
    PrintData data;
    printData[PrintType::PrintTypeOffer] = data;
  }

  void PrepareInvoice()
  {
    PrintData data;
    printData[PrintType::PrintTypeInvoice] = data;
  }

  void PrepareSingleOffer()
  {
    PrintData data;

    data.tableCols = std::string("<table><tr>")
      + "<th>Pos.</th>"
      + "<th>Bezeichnung</th>"
      + "<th>Menge</th>"
      + "<th>Einheit</th>"
      + "<th>Einzelpreis</th>"
      + "<th>SUMME</th>"
      += "</tr></table>";

    printData[PrintType::PrintTypeSingleOffer] = data;
  }

  void PrepareSingleInvoice()
  {
    PrintData data;
    printData[PrintType::PrintTypeSingleInvoice] = data;
  }
}

Export::Export(PrintType const &type)
  : m_type(type)
{
  PrintTitle();
  PrintHeader();
}

void Export::Prepare()
{
  PrepareAddress();
  PrepareMaterial();
  PrepareService();
  PrepareOffer();
  PrepareInvoice();
  PrepareSingleOffer();
  PrepareSingleInvoice();
}

std::string Export::operator()(QSqlQuery const &query)
{
  PrintData(query);

  return m_title + m_header + m_tableCols + m_data + m_ending;
}

void Export::PrintTitle()
{
  m_title = printData[m_type].title;
}

void Export::PrintTableCols()
{
  m_header = printData[m_type].tableCols;
}

void Export::PrintHeader()
{

}

void Export::PrintData(QSqlQuery const &query)
{

}

void Export::PrintEnding(QString const &ending)
{

}
