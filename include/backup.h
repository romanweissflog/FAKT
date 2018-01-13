#ifndef BACKUP_H
#define BACKUP_H

#include "QtCore/qsettings.h"

#include <string>

class Backup
{
public:
  void operator()(QSettings *settings);
};

#endif
