#include "functionality\export.h"
#include "pages\general_print_page.h"
#include "functionality\utils.hpp"

#include "QtSql\qsqlerror.h"
#include "QtGui\qtexttable.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace
{
  struct QueryColumnData
  {
    QString name;
    int pos;
    QTextLength constrain;
    Qt::Alignment alignment;
  };

  struct QueryData
  {
    int placeHolderPos;
    std::vector<int> valuePos;
    std::vector<int> textPos;
    std::vector<QueryColumnData> columns;
  };
  using TypePositions = std::map<uint8_t, QueryData>;
  
  std::map<PrintType, TypePositions> queryData
  {
    { PrintType::PrintTypeOffer, 
      { 
        { 1U , 
          {
            2,
            { 5, 6, 10 },
            { 1, 3, 4 },
            {
              { "Pos.   ",                                                1, QTextLength(QTextLength::FixedLength, 58), Qt::AlignLeft  },
              { "Bezeichnung                                           ", 3, QTextLength(QTextLength::FixedLength, 316), Qt::AlignLeft  },
              { "    Menge     ",                                         5, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "Einheit   ",                                             4, QTextLength(QTextLength::FixedLength, 66),  Qt::AlignLeft  },
              { "Einzelpreis   ",                                         6, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "   SUMME    ",                                          10, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight }
            }
          }
        },
        { 2U,
          {
            2,
            { 5 },
            { 3, 4 },
            {
              { "Bezeichnung                                                                                         ", 3, QTextLength(QTextLength::FixedLength, 550), Qt::AlignLeft  },
              { "   Menge     ",                                                                                        5, QTextLength(QTextLength::FixedLength, 100), Qt::AlignRight },
              { " Einheit   ",                                                                                          4, QTextLength(QTextLength::FixedLength, 100), Qt::AlignLeft  }
            }
          }
        }
      }
    }, 
    { PrintType::PrintTypeInvoice,
      {
        { 1U ,
          {
            2,
            { 5, 6, 10 },
            { 1, 3, 4 },
            {
              { "Pos.   ",                                                1, QTextLength(QTextLength::FixedLength, 58), Qt::AlignLeft },
              { "Bezeichnung                                           ", 3, QTextLength(QTextLength::FixedLength, 316), Qt::AlignLeft },
              { "    Menge     ",                                         5, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "Einheit   ",                                             4, QTextLength(QTextLength::FixedLength, 66),  Qt::AlignLeft },
              { "Einzelpreis   ",                                         6, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "   SUMME    ",                                          10, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight }
            }
          }
        },
        { 2U,
          {
            2,
            { 5 },
            { 3, 4 },
            {
              { "Bezeichnung                                                                                         ", 3, QTextLength(QTextLength::FixedLength, 550), Qt::AlignLeft },
              { "   Menge     ",                                                                                        5, QTextLength(QTextLength::FixedLength, 100), Qt::AlignRight },
              { " Einheit   ",                                                                                          4, QTextLength(QTextLength::FixedLength, 100), Qt::AlignLeft }
            }
          }
        }
      }
    },
    { PrintType::PrintTypeJobsite,
      {
        { 1U ,
          {
            2,
            { 5, 6, 10 },
            { 1, 3, 4 },
            {
              { "Pos.   ",                                                1, QTextLength(QTextLength::FixedLength, 58), Qt::AlignLeft },
              { "Bezeichnung                                           ", 3, QTextLength(QTextLength::FixedLength, 316), Qt::AlignLeft },
              { "    Menge     ",                                         5, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "Einheit   ",                                             4, QTextLength(QTextLength::FixedLength, 66),  Qt::AlignLeft },
              { "Einzelpreis   ",                                         6, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight },
              { "   SUMME    ",                                          10, QTextLength(QTextLength::FixedLength, 96),  Qt::AlignRight }
            }
          }
        },
        { 2U,
          {
            2,
            { 5 },
            { 3, 4 },
            {
              { "Bezeichnung                                                                                         ", 3, QTextLength(QTextLength::FixedLength, 550), Qt::AlignLeft },
              { "   Menge     ",                                                                                        5, QTextLength(QTextLength::FixedLength, 100), Qt::AlignRight },
              { " Einheit   ",                                                                                          4, QTextLength(QTextLength::FixedLength, 100), Qt::AlignLeft }
            }
          }
        }
      }
    },
  };

  QString SetLineBreaks(QString const &input)
  {
    if (input.size() < 10)
    {
      return input;
    }

    std::string txt = input.toStdString();
    std::replace(txt.begin(), txt.end(), '\n', ' ');
    txt += "\n";

    return QString::fromStdString(txt);
  }

  QString RoundNumber(double value)
  {
    return QString::number(std::round(value * 100) / 100);
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

  QString FillNumber(uint8_t subType, PrintData const &data)
  {
    QString txt = "";
    switch (subType)
    {
    case 1: txt += data.what + " Nr.: " + data.number; break;
    case 2: txt += "Lieferschein " + data.number;
    default: break;
    }    
    return txt;
  }

  QString FillDate(PrintData const &data)
  {
    QString txt = data.date + "\n";
    txt += QString("____________________________________________________________________________________________\n\n");
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

  void DrawDashedLine(QTextCursor &cursor)
  {
    QTextBlockFormat format;
    format.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    format.setAlignment(Qt::AlignCenter);
    cursor.insertBlock(format);
    cursor.insertText("--------------------------------------------------------------------------------------------------------");
  }
}

Export::Export(PrintType const &type)
  : m_type(type)
{
}

ReturnValue Export::operator()(QTextCursor &cursor, PrintData const &data, QSqlQuery &dataQuery, std::string const &logo)
{
  uint8_t subType = PrintSubType::None;
  GeneralPrintPage *page = new GeneralPrintPage(data, subType);
  connect(page, &GeneralPrintPage::Close, [this]()
  {
    emit Close();
  });
  emit Created(page);
  if (page->exec() == QDialog::Accepted)
  {
    if (logo.size() != 0)
    {
      QTextImageFormat imageFormat;
      imageFormat.setName(QString::fromStdString(logo));
      cursor.insertImage(imageFormat);
    }
    PrintHeader(cursor, subType, data);
    PrintQuery(cursor, subType, dataQuery);
    PrintResult(cursor, subType, data);
    PrintEnding(cursor, subType, data);

    emit Close();
    return ReturnValue::ReturnSuccess;
  }
  emit Close();
  return ReturnValue::ReturnAbort;
}

void Export::PrintHeader(QTextCursor &cursor, uint8_t subType, PrintData const &data)
{
  QTextBlockFormat format;
  format.setIndent(2);
  format.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format);
  cursor.insertText(FillHeader(data));

  QTextBlockFormat format2;
  QTextCharFormat charFormat2;
  charFormat2.setFont(QFont("Times", 12, QFont::Bold));
  format2.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format2, charFormat2);
  cursor.insertText(FillNumber(subType, data));

  QTextBlockFormat format3;
  format3.setLayoutDirection(Qt::LayoutDirection::RightToLeft);
  cursor.insertBlock(format3, QTextCharFormat());
  cursor.insertText(FillDate(data));

  //QTextBlockFormat format4;
  //format4.setIndent(2);
  //format4.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  //cursor.insertBlock(format4);
  //cursor.insertText(FillTop(data));
}

void Export::PrintQuery(QTextCursor &cursor, uint8_t subType, QSqlQuery &query)
{
  QVector<QTextLength> constrains;
  for (auto &&p : queryData[m_type][subType].columns)
  {
    constrains.push_back(p.constrain);
  }
  QTextTableFormat format;
  format.setAlignment(Qt::AlignCenter);
  format.setHeaderRowCount(1);
  format.setCellSpacing(4.0);
  format.setColumnWidthConstraints(constrains);
  format.setWidth(QTextLength(QTextLength::Type::FixedLength, 750));
  format.setBorderStyle(QTextFrameFormat::BorderStyle::BorderStyle_None);

  int32_t count{};
  std::vector<util::TablePosNumber<5>> positions;
  while(query.next())
  {
    positions.push_back(util::TablePosNumber<5>(query.value(1).toString().toStdString()));
    count++;
  }
  query.first();

  std::vector<int32_t> idx(positions.size());
  iota(idx.begin(), idx.end(), 0);

  std::sort(idx.begin(), idx.end(), [&positions](int32_t i1, int32_t i2) {return positions[i1] < positions[i2]; });
  
  QTextTable *table = cursor.insertTable(count + 1, static_cast<int>(queryData[m_type][subType].columns.size()), format);
  int32_t k{};
  QTextCharFormat headerFormat;
  headerFormat.setFontUnderline(true);
  headerFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::DashUnderline);
  for (auto &&p : queryData[m_type][subType].columns)
  {
    auto cell = table->cellAt(0, k);
    cell.setFormat(headerFormat);
    cursor = cell.firstCursorPosition();
    cursor.insertText(p.name);
    ++k;
  }

  int32_t i{};
  QTextBlockFormat colFormat;
  while (true)
  {
    int32_t j{};
    auto &&data = queryData[m_type][subType];
    auto const isPlaceholder = (query.value(data.placeHolderPos).toString() == QString("#"));
    for (auto &&p : data.columns)
    {
      cursor = table->cellAt(idx[i] + 1, j).firstCursorPosition();
      colFormat.setAlignment(p.alignment);
      cursor.setBlockFormat(colFormat);
      auto const isNoText = (std::find(std::begin(data.textPos), std::end(data.textPos), p.pos) == std::end(data.textPos));
      auto const isNumber = std::find(std::begin(data.valuePos), std::end(data.valuePos), p.pos) != std::end(data.valuePos);
      if (isNoText && isPlaceholder)
      {
        cursor.insertText("");
      }
      else
      {
        if (isNumber)
        {
          cursor.insertText(RoundNumber(query.value(p.pos).toDouble()));
        }
        else
        {
          cursor.insertText(SetLineBreaks(query.value(p.pos).toString()));
        }
      }
      ++j;
    }
    ++i;
    if (!query.next())
    {
      break;
    }
  }
  cursor.setPosition(table->lastPosition() + 1);
}

void Export::PrintResult(QTextCursor &cursor, uint8_t subType, PrintData const &data)
{
  DrawDashedLine(cursor);

  QTextBlockFormat format;
  format.setAlignment(Qt::AlignRight);
  format.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  cursor.insertBlock(format);
  cursor.insertText(FillTotal(data));

  DrawDashedLine(cursor);
}

void Export::PrintEnding(QTextCursor &cursor, uint8_t subType, PrintData const &data)
{
  QTextBlockFormat format;
  cursor.insertBlock(format);
  cursor.insertText(FillEnding(data));
}
