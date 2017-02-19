#include "fakt.h"
#include "ui_fakt.h"

Fakt::Fakt(QWidget *parent)
  : QMainWindow(parent)
  , m_ui(new Ui::fakt)
{
  m_ui->setupUi(this);
}

Fakt::~Fakt()
{}