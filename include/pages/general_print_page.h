#ifndef GENERAL_PRINT_PAGE_H
#define GENERAL_PRINT_PAGE_H

#include "functionality\utils.h"
#include "functionality\data_entries.h"

#include "QtWidgets\qwidget.h"
#include "QtGui\qevent.h"

#include <cstdint>

class PrintSubType
{
public:
  static const uint8_t None = 0;
  static const uint8_t Type = 1;
  static const uint8_t DeliveryNote = 2;
  static const uint8_t MeasureListEmpty = 7;
  static const uint8_t MeasureList = 8;
};

class PrintSubTypeInvoice : public PrintSubType
{
public:
  static const uint8_t TimeList = 13;
};

class PrintSubTypeJobsite : public PrintSubType
{
public:
  static const uint8_t TimeList = 23;
  static const uint8_t Confirmation = 24;
  static const uint8_t Accounting = 25;
};

class PrintSubTypeOffer : public PrintSubType
{
public:
  static const uint8_t TimeListShort = 33;
  static const uint8_t TimeListLong = 34;
  static const uint8_t Order = 35;
  static const uint8_t Confirmation = 36;
  static const uint8_t Inquiry = 39;
};

namespace Ui
{
  class generalPrintPage;
}

class GeneralPrintPage : public QDialog
{
  Q_OBJECT
public:
  GeneralPrintPage(TabName const &parentTab, GeneralMainData const &data, uint8_t &subType, QWidget *parent = nullptr);
  ~GeneralPrintPage() = default;

protected:
  void keyPressEvent(QKeyEvent *e) override;

signals:
  void Close();

private:
  void SetInvoiceData();
  void SetJobsiteData();
  void SetOfferData();

public:

private:
  Ui::generalPrintPage *m_ui;
  size_t m_logId;
  uint8_t &m_chosenSubType;
};

#endif
