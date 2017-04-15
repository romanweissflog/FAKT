#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "QtCore\qstring.h"

struct Settings
{
  double euroPerMin;
  double mwst;
  double hourlyRate;
  std::string lastInvoice;
  std::string lastOffer;
};

struct ServiceData
{
  QString key;
  QString description;
  QString unit;
  double ep;
  double service;
  double material;
  double helperMaterial;
  double minutes;
  double ekp;
};

struct MaterialData
{
  QString key;
  QString description;
  QString unit;
  double netto;
  double brutto;
  double ekp;
  double minutes;
  double stockSize;
  QString supplier;
  double ep;
};

struct AdressData
{
  QString key;
  uint32_t number;
  QString salutation;
  QString name;
  QString street;
  QString plz;
  QString city;
  QString phone1;
  QString phone2;
  QString phone3;
  QString fax;
  QString mail;
  double yearNetto;
  double brutto;
  bool epUeb;
};

struct GeneralInputData
{
  int64_t invoiceNumber;
  int64_t pos;
  double totalProfitMatPerc;
  double totalProfitMatEuro;
};

struct GeneralData
{
  int64_t pos;
  QString artNr;
  QString text;
  double profitMatPerc;
  double profitMatEuro;
  double workTime;
  QString unit;
  uint32_t number;
  bool isMaterial;
  double ekp;
  double surcharge;
  double material;
  double discount;
  bool isService;
  double hourlyRate;
  double time;
  double service;
  double corrFactor;
  double helpMat;
  double ep;
  double total;
};
#endif
