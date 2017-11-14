/**
* @file data_entries.h
*/

#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "QtCore\qstring.h"

/**
* @class Struct for all settings to be read from settings file
*/
struct Settings
{
  double euroPerMin;
  double mwst;
  double hourlyRate;
  std::string lastInvoice;
  std::string lastOffer;
  std::string logFile;
  QString defaultHeading;
};


/**
* @class Empty parent class for internal used data
*/
struct Data
{};


/**
* @class Struct for service data
*/
struct ServiceData : public Data
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


/**
* @class Struct for material data
*/
struct MaterialData : public Data
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


/**
* @class Struct for address data
*/
struct AddressData : public Data
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


/**
* @class Struct for an entry (material or service) used by every invoice or offering
*/
struct GeneralData : public Data
{
  QString pos;
  QString artNr;
  QString text;
  QString unit;
  uint32_t number;
  double ekp;
  double surcharge;
  double material;
  double discount;
  double hourlyRate;
  double time;
  double service;
  double corrFactor;
  double helpMat;
  double ep;
  double total;
};


/**
* @class Struct for all invoice data
*/
struct InvoiceData : public Data
{
  int64_t invoiceNumber;
  QString invoiceDate;
  QString salutation;
  QString name;
  QString street;
  QString place;
  double materialTotal;
  double serviceTotal;
  double helperTotal;
  double total;
  double mwstTotal;
  double brutto;
  double discount;
  double discountTotal;
  double paid;
  double skonto;
  double skTotal;
  QString headline;
  QString payDate;
  uint32_t customerNumber;
  QString deliveryDate;
  QString endline;
  bool printed;
  QString subject;
  double mwst;
  double account;
  double discountExtra;
  QString weu;
};

#endif
