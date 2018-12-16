#ifndef YEAR_WATCHDOG_H
#define YEAR_WATCHDOG_H

#include "data_entries.h"

class YearWatchdog
{
public:
  void operator()(Settings &settings);
};

#endif
