#include "functionality\fakt.h"

#include "QtWidgets\qapplication.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include <tlhelp32.h>

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

bool ProcessAlreadyRunning()
{
  PROCESSENTRY32 entry;
  entry.dwSize = sizeof(PROCESSENTRY32);

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  DWORD ownPid = GetCurrentProcessId();
  bool isRunning{};
  if (Process32First(snapshot, &entry) == TRUE)
  {
    while (Process32Next(snapshot, &entry) == TRUE)
    {
      if (stricmp(entry.szExeFile, "FAKT.exe") == 0
         && entry.th32ProcessID != ownPid)
      {
        isRunning = true;
        break;
      }
    }
  }

  CloseHandle(snapshot);
  return isRunning;
}

int main(int argc, char* argv[])
{
  if (ProcessAlreadyRunning())
  {
    return 0;
  }

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