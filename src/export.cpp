#include "include/export.h"

#include "QtSql\qsqlerror.h"
#include "QtGui\qtexttable.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace
{
  struct Positions
  {
    std::vector<QString> columns;
    std::vector<int> data;
  };

  std::map<PrintType, Positions> queryData
  {
    { PrintType::PrintTypeOffer, 
      {
        { "Pos.", "Bezeichnung", "Menge", "Einheit", "Einzelpreis", "SUMME" },
        { 1, 3, 5, 4, 6, 10 }
      }
    }
  };

  QString FillHeader(PrintData const &data)
  {
    QString txt = "";
    if (data.salutation.size() != 0)
    {
      txt += data.salutation + " ";
    }
    txt += data.name + "\n";
    txt += data.street + "\n" + data.place + "\n";
    if (data.headline.size() != 0)
    {
      txt += data.headline + "\n";
    }
    if (data.subject.size() != 0)
    {
      txt += data.subject + "\n";
    }
    txt += "\n";
    return txt;
  }

  QString FillNumber(PrintData const &data)
  {
    QString txt = "";
    txt += data.what + " Nr.: " + data.number + "      " + data.date + "\n";
    return txt;
  }

  QString FillTotal(PrintData const &data)
  {
    QString txt = "";
    txt += "Netto-Summe  (EUR): " + QString::number(data.netto) + "\n";
    txt += QString::number(data.mwst) + "% Mwst.  (EUR): " + QString::number(data.mwstPrice) + "\n";
    txt += "Gesamt-Summe (EUR): " + QString::number(data.brutto) + "\n";
    return txt;
  }

  QString FillEnding(PrintData const &data)
  {
    return data.endline + "\n";
  }
}

Export::Export(PrintType const &type)
  : m_type(type)
{
}

void Export::operator()(QTextCursor &cursor, PrintData const &data, QSqlQuery &dataQuery, std::string const &logo)
{
  if (logo.size() != 0)
  {
    QTextImageFormat imageFormat;
    imageFormat.setName("mydata://" + QString::fromStdString(logo));
    cursor.insertImage(imageFormat);
  }
  PrintHeader(cursor, data);
  PrintQuery(cursor, dataQuery);
  PrintResult(cursor, data);
  PrintEnding(cursor, data);
}

void Export::PrintHeader(QTextCursor &cursor, PrintData const &data)
{
  QTextBlockFormat format;
  cursor.insertBlock(format);
  cursor.insertText(FillHeader(data));

  QTextBlockFormat format2;
  cursor.insertBlock(format2);
  cursor.insertText(FillNumber(data));
}

void Export::PrintQuery(QTextCursor &cursor, QSqlQuery &query)
{
  QTextTableFormat format;
  format.setAlignment(Qt::AlignCenter);
  format.setHeaderRowCount(1);
  format.setCellSpacing(0);
  
  int32_t count{};
  while(query.next())
  {
    count++;
  }
  query.first();
  
  QTextTable *table = cursor.insertTable(count + 1, static_cast<int>(queryData[m_type].columns.size()), format);
  int32_t k{};
  for (auto &&p : queryData[m_type].columns)
  {
    cursor = table->cellAt(0, k).firstCursorPosition();
    cursor.insertText(p);
    ++k;
  }

  int32_t i = 1;
  while (query.next())
  {
    int32_t j{};
    for (auto &&p : queryData[m_type].data)
    {
      cursor = table->cellAt(i, j).firstCursorPosition();
      cursor.insertText(query.value(p).toString());
      ++j;
    }
    ++i;
  }
  cursor.setPosition(table->lastPosition() + 1);
}

void Export::PrintResult(QTextCursor &cursor, PrintData const &data)
{
  QTextBlockFormat format;
  cursor.insertBlock(format);
  cursor.insertText(FillTotal(data));
}

void Export::PrintEnding(QTextCursor &cursor, PrintData const &data)
{
  QTextBlockFormat format;
  cursor.insertBlock(format);
  cursor.insertText(FillEnding(data));
}
