#ifndef DATA_ENTRIES_H
#define DATA_ENTRIES_H

#include "QtCore\qstring.h"

struct Settings
{
  double euroPerMin;
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

#endif