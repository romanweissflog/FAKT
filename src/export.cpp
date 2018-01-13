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
    QVector<QTextLength> constrains;
  };
  
  std::map<PrintType, Positions> queryData
  {
    { PrintType::PrintTypeOffer, 
      {
        { "Pos.", "Bezeichnung", "Menge", "Einheit", "Einzelpreis", "SUMME" },
        { 1, 3, 5, 4, 6, 10 },
        {
          QTextLength(QTextLength::FixedLength, 30),
          QTextLength(QTextLength::FixedLength, 600),
          QTextLength(QTextLength::FixedLength, 30),
          QTextLength(QTextLength::FixedLength, 30),
          QTextLength(QTextLength::FixedLength, 30),
          QTextLength(QTextLength::FixedLength, 30),
        }
      }
    }
  };

  QString SetLineBreaks(QString const &input)
  {
    if (input.size() < 10)
    {
      return input;
    }

    std::string txt = input.toStdString();
    //std::replace(txt.begin(), txt.end(), "\n", " ");

    return QString::fromStdString(txt);
  }

  QString FillHeader(PrintData const &data)
  {
    QString txt = "";
    if (data.salutation.size() != 0)
    {
      txt += data.salutation + " ";
    }
    txt += data.name + "\n";
    txt += data.street + "\n" + data.place + "\n";
    txt += "\n";
    return txt;
  }

  QString FillNumber(PrintData const &data)
  {
    QString txt = "";
    txt += data.what + " Nr.: " + data.number;
    return txt;
  }

  QString FillDate(PrintData const &data)
  {
    QString txt = data.date + "\n";
    return txt;
  }

  QString FillTop(PrintData const &data)
  {
    QString txt = "";
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

  QString FillTotal(PrintData const &data)
  {
    auto toStringPrice = [](double val) -> QString
    {
      std::string priceString = std::to_string(val);
      priceString = priceString.substr(0, priceString.find_last_of(".") + 3); 
      priceString.insert(priceString.begin(), 10 - priceString.size(), ' ');
      return QString::fromStdString(priceString);
    };
    QString txt = "";
    txt += "Netto-Summe  (EUR): " + toStringPrice(data.netto) + "\n";
    txt += QString::number(data.mwst) + "% Mwst.    (EUR): " + toStringPrice(data.mwstPrice) + "\n";
    txt += "Gesamt-Summe (EUR): " + toStringPrice(data.brutto) + "\n";
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
    imageFormat.setName(QString::fromStdString(logo));
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
  format.setIndent(2);
  format.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format);
  cursor.insertText(FillHeader(data));

  QTextBlockFormat format2;
  format2.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format2);
  cursor.insertText(FillNumber(data));

  QTextBlockFormat format3;
  format3.setLayoutDirection(Qt::LayoutDirection::RightToLeft);
  cursor.insertBlock(format3);
  cursor.insertText(FillDate(data));

  QTextBlockFormat format4;
  format4.setIndent(2);
  format4.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format4);
  cursor.insertText(FillTop(data));
}

void Export::PrintQuery(QTextCursor &cursor, QSqlQuery &query)
{
  QTextTableFormat format;
  format.setAlignment(Qt::AlignCenter);
  format.setHeaderRowCount(1);
  format.setCellSpacing(0);
  format.setColumnWidthConstraints(queryData[m_type].constrains);
  format.setWidth(QTextLength(QTextLength::Type::FixedLength, 750));
  format.setBorderStyle(QTextFrameFormat::BorderStyle::BorderStyle_None);
  
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
      cursor.insertText(SetLineBreaks(query.value(p).toString()));
      ++j;
    }
    ++i;
  }
  cursor.setPosition(table->lastPosition() + 1);
}

void Export::PrintResult(QTextCursor &cursor, PrintData const &data)
{
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignRight);
  format.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format);
  cursor.insertText(FillTotal(data));
}

void Export::PrintEnding(QTextCursor &cursor, PrintData const &data)
{
  QTextBlockFormat format;
  cursor.insertBlock(format);
  cursor.insertText(FillEnding(data));
}
