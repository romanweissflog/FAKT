#include "functionality\fakt.h"

#include "QtWidgets\qapplication.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  if (argc == 1)
  {
    std::cout << "BAD Usage: fakt.exe <settings.ini>" << std::endl;
    return -1;
  }
  Fakt fakt;
  fakt.SetSettings(std::string(argv[1]));
  fakt.show();

  app.exec();
  return 0;
}