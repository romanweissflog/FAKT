#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "QtCore\qstring.h"

struct Settings
{
  double euroPerMin;
  double mwst;
};

struct LeistungData
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

#endif