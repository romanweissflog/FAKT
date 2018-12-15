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

namespace printmask
{
  static uint16_t const Undef     = 0x0000;
  static uint16_t const Offer     = 0x0001;
  static uint16_t const Jobsite   = 0x0002; 
  static uint16_t const Invoice   = 0x0004;
  static uint16_t const Pdf       = 0x0010; // currently unusued
  static uint16_t const Print     = 0x0020; // currently unusued
  static uint16_t const Short     = 0x0100;
  static uint16_t const Long      = 0x0200;
  static uint16_t const Position  = 0x1000;
  static uint16_t const Groups    = 0x2000;
  static uint16_t const All       = 0x4000;
}

class Export : public QWidget
{
  Q_OBJECT
public:
  Export(uint16_t mask = 0x0000, QWidget *parent = nullptr);
  virtual ~Export() = default;
  ReturnValue operator()(TabName const &parentTab, 
    QSqlQuery const &mainQuery, 
    QSqlQuery const &dataQuery, 
    QSqlQuery const &groupQuery,
    QSqlQuery const &extraQuery,
    uint16_t withLogo);

signals:
  void Created(QWidget *page);
  void Close();

private:
  LimeReport::ReportEngine *m_report;
  uint16_t m_mask;
  size_t m_logId;
};

#endif
