#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "utils.h"

#include "QtCore\qstring.h"

#include <vector>

struct Constants
{
  int rowOffset;
};

struct Settings
{
  double mwst;
  double hourlyRate;
  std::string lastInvoice;
  std::string lastOffer;
  std::string lastJobsite;
  std::string lastCustomer;
  std::string logFile;
  QString defaultHeadline;
  QString defaultInvoiceEndline;
  QString defaultOfferEndline;
  QString skontoTextShort;
  QString skontoTextLong;
  QString discountText;
  Constants constants;
};


struct SingleData
{
  QString column;
  QVariant entry;
};

using DatabaseDataPair = std::pair<QString, SingleData>;
using DatabaseDataEntry = std::vector<DatabaseDataPair>;

struct DatabaseData
{
  DatabaseDataEntry data;

  SingleData& operator [](QString const &key);
  SingleData operator [](QString const &key) const;
};

#endif
