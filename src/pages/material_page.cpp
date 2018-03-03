#include "pages\material_page.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"

#include "ui_material_page.h"

MaterialPage::MaterialPage(Settings *settings,
  QSqlQuery &query,
  QString const &edit,
  QWidget *parent)
  : ParentPage("MaterialPage", parent)
  , m_ui(new Ui::materialPage)
  , m_mwst(settings->mwst)
  , m_hourlyRate(settings->hourlyRate)
  , m_query(query)
{
  m_ui->setupUi(this);
  data = {};
  connect(m_ui->editKey, &QLineEdit::textChanged, [this](QString txt)
  {
    data.key = txt;
  });
  connect(m_ui->editMainDescription, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mainDescription = txt;
  });
  connect(m_ui->editDescr, &QTextEdit::textChanged, [this]()
  {
    data.description = m_ui->editDescr->toPlainText();
  });
  connect(m_ui->editUnit, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editNetto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.netto = l.toDouble(txt);
    data.brutto = data.netto + m_mwst / 100 * data.netto;
    m_ui->editEkp->setText(l.toString(data.netto, 'f', 2));
    m_ui->editBrutto->setText(l.toString(data.brutto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editBrutto, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.brutto = l.toDouble(txt);
    //data.netto = data.brutto * 100 / (100 + m_mwst);
    //m_ui->editNetto->setText(l.toString(data.netto, 'f', 2));
    Calculate();
  });
  connect(m_ui->editEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.ekp = l.toDouble(txt);
  });
  connect(m_ui->editMinutes, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.minutes = l.toDouble(txt);
    Calculate();
  });

  m_query.clear();
  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT ARTNR FROM MATERIAL"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
  if (edit.size() > 0)
  {
    CopyData(edit);
    Calculate();
  }
}

MaterialPage::~MaterialPage()
{}

void MaterialPage::Calculate()
{
  QLocale l(QLocale::German);
  double value = data.netto + data.minutes / 60.0 * m_hourlyRate;
  m_ui->labelTotal->setText(l.toString(value, 'f', 2));
}

void MaterialPage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0 && txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID"))
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  m_query.next();
  QLocale l(QLocale::German);
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editMainDescription->setText(m_query.value(2).toString());
  m_ui->editDescr->setText(m_query.value(3).toString());
  m_ui->editUnit->setText(m_query.value(4).toString());
  m_ui->editNetto->setText(l.toString(m_query.value(6).toDouble(), 'f', 2));
  m_ui->editMinutes->setText(l.toString(m_query.value(7).toDouble(), 'f', 2));
  m_ui->editBrutto->setText(l.toString(m_query.value(8).toDouble(), 'f', 2));
  m_ui->editEkp->setText(l.toString(m_query.value(5).toDouble(), 'f', 2));
  m_ui->labelTotal->setText(l.toString(m_query.value(9).toDouble(), 'f', 2));
}

void MaterialPage::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}
