#ifndef GENERAL_PRINT_PAGE_H
#define GENERAL_PRINT_PAGE_H

#include "functionality\utils.h"
#include "functionality\data_entries.h"

#include "QtWidgets\qwidget.h"
#include "QtGui\qevent.h"

#include <cstdint>

namespace Ui
{
  class generalPrintPage;
}

class GeneralPrintPage : public QDialog
{
  Q_OBJECT
public:
  GeneralPrintPage(TabName const &parentTab, DatabaseData const &data, uint16_t &subMask, QWidget *parent = nullptr);
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
  uint16_t &m_subMask; 
  size_t m_logId;
};

#endif
