/**
* @file data_entries.h
*/

#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "QtCore\qstring.h"

#include <memory>


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
  std::string lastJobsite;
  std::string logFile;
  QString defaultHeading;
  std::string logoFile;
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
  QString phone;
  QString fax;
  QString mail;
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
  double helpMat;
  double ep;
  double total;
};


/**
* @class Struct for all invoice data
*/
struct GeneralMainData : public Data
{
  QString number;
  QString customerNumber;
  QString date;
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
  double skonto;
  double payNormal;
  double paySkonto;
  double hourlyRate;
  QString headline;
  QString endline;
  QString subject;
};


struct OfferData
{
  std::shared_ptr<GeneralMainData> baseData;
  QString deadLine;
};


struct InvoiceData
{
  std::shared_ptr<GeneralMainData> baseData;
  double skontoTotal;
  double paid;
  QString payDate;
  QString deliveryDate;
  double mwst;
};

#endif
