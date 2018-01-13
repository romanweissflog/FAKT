#include "backup.h"

#include <chrono>
#include <cstdint>
#include <experimental/filesystem>
#include <direct.h>

void Backup::operator()(QSettings *settings)
{
  static int64_t period = 7 * 24;

  using namespace std::chrono;
  auto now = duration_cast<hours>(system_clock::now().time_since_epoch()).count();
  auto last = settings->value("lastBackup").toLongLong();
  if (now - last < period)
  {
    return;
  }

  std::string folder = "backups";
  _mkdir(folder.c_str());
  folder = folder + "/" + std::to_string(now);
  _mkdir(folder.c_str());

  std::experimental::filesystem::copy_file("fakt.db", folder + "/fakt.db");
  std::experimental::filesystem::copy_file("offers.db", folder + "/offers.db");
  std::experimental::filesystem::copy_file("invoices.db", folder + "/invoices.db");
  std::experimental::filesystem::copy_file("jobsites.db", folder + "/jobsites.db");

  settings->setValue("lastBackup", now);
}