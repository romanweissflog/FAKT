/**
* @file data_entries.h
*/

#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "utils.h"

#include "QtCore\qstring.h"

#include <memory>


/**
* @class Struct for all settings to be read from settings file
*/
struct Settings
{
  double mwst;
  double hourlyRate;
  std::string lastInvoice;
  std::string lastOffer;
  std::string lastJobsite;
  std::string lastCustomer;
  std::string logFile;
  std::string logoFile;
  std::string defaultHeadline;
  std::string defaultEndline;
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
  ServiceData() = default;

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
  MaterialData() = default;

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
  AddressData() = default;

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
  GeneralData() = default;

  QString pos;
  QString artNr;
  QString text;
  uint32_t number;
  double ep;
  double total;
  QString unit;
  double helpMat;
  double time;
  double discount;
  double ekp;
  double surcharge;
  double hourlyRate;
};


/**
* @class Struct for all invoice data
*/
struct GeneralMainData : public Data
{
  GeneralMainData() = default;

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


struct OfferData : public GeneralMainData
{
  OfferData() = default;

  QString deadLine;
};


struct InvoiceData : public GeneralMainData
{
  InvoiceData() = default;

  double skontoTotal;
  double paid;
  QString payDate;
  QString deliveryDate;
  double mwst;
};


struct PrintData
{
  TabName tab;
  QString what;
  QString salutation;
  QString name;
  QString street;
  QString place;
  QString number;
  QString date;
  double mwst;
  double netto;
  double mwstPrice;
  double brutto;
  QString headline;
  QString subject;
  QString endline;
};

#endif
