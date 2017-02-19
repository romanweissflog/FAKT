#ifndef FAKT_H
#define FAKT_H

#include "QtWidgets\qmainwindow.h"

class Fakt : public QMainWindow
{
  Q_OBJECT

public:
  Fakt(QWidget *parent = nullptr);
  ~Fakt();
};

#endif