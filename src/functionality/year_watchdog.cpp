#include "functionality\year_watchdog.h"

#include "qmessagebox.h"

#include <chrono>

void YearWatchdog::operator()(Settings &settings)
{
  using namespace std::chrono;
  system_clock::time_point now = system_clock::now();
  time_t tt = system_clock::to_time_t(now);
  tm utc_tm = *gmtime(&tt);
  tm local_tm = *localtime(&tt);
  int64_t year = local_tm.tm_year + 1900;

  if (year != settings.currentYear)
  {
    settings.currentYear = year;

    QMessageBox *question = new QMessageBox();
    std::string msg = "Das Jahr hat sich ge" + german::ae + "ndert.\nAnpassen der n" 
      + german::ae + "chsten Rechnungsnummern?";
    question->setWindowTitle("INFORMATION");
    question->setText(QString::fromStdString(msg));
    question->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    question->setDefaultButton(QMessageBox::No);
    question->setButtonText(QMessageBox::Yes, "Ja");
    question->setButtonText(QMessageBox::No, "No");
    if (question->exec() == QMessageBox::Yes)
    {
      std::string numberString = std::to_string(year % 100) + "0000";
      settings.lastOffer = numberString;
      settings.lastInvoice = numberString;
    }
  }
}