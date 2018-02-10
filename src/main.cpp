#include "functionality\fakt.h"

#include "QtWidgets\qapplication.h"

#include <iostream>
#include <string>
#include <Windows.h>

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  if (argc < 3)
  {
    std::cout << "BAD Usage: fakt.exe <settings.ini> <splashscreen.bmp>" << std::endl;
    return -1;
  }
  QPixmap splashImage(argv[2]);
  QSplashScreen splash;

  Fakt fakt(&splash);
  auto scaledImage = splashImage.scaledToHeight(fakt.height()).scaledToWidth(fakt.width());
  splash.setPixmap(scaledImage);
  splash.setFont(QFont("Times", 16, QFont::Bold));

  splash.show();
  app.processEvents();

  fakt.SetSettings(std::string(argv[1]));
  fakt.Init();
  fakt.show();
  splash.finish(&fakt);

  app.exec();
  return 0;
}