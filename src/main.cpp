#include "fakt.h"

#include "QtWidgets\qapplication.h"

int main(int argv, char **args)
{
  QApplication app(argv, args);

  Fakt fakt;
  fakt.show();

  app.exec();
  return 0;
}