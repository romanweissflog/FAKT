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
    data.netto = txt.toDouble();
    data.brutto = data.netto + m_mwst / 100 * data.netto;
    m_ui->editBrutto->setText(QString::number(data.brutto));
    Calculate();
  });
  connect(m_ui->editBrutto, &QLineEdit::textChanged, [this](QString txt)
  {
    data.brutto = txt.toDouble();
    data.netto = data.brutto * 100 / (100 + m_mwst);
    m_ui->editNetto->setText(QString::number(data.netto));
    Calculate();
  });
  connect(m_ui->editEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
  });
  connect(m_ui->editMinutes, &QLineEdit::textChanged, [this](QString txt)
  {
    data.minutes = txt.toDouble();
    Calculate();
  });

  m_query.clear();
  m_ui->copyBox->addItem("");
  if (!m_query.exec("SELECT ARTNR FROM MATERIAL"))
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
  if (edit.size() > 0)
  {
    CopyData(edit);
  }
}

MaterialPage::~MaterialPage()
{}

void MaterialPage::Calculate()
{
  double value = data.netto + data.minutes / 60.0 * m_hourlyRate;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void MaterialPage::CopyData(QString txt)
{
  if (m_ui->copyBox->currentIndex() == 0 && txt.size() == 0)
  {
    return;
  }
  if (!m_query.prepare("SELECT * FROM MATERIAL WHERE ARTNR = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", txt);
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  m_ui->editKey->setText(m_query.value(1).toString());
  m_ui->editDescr->setText(m_query.value(2).toString());
  m_ui->editUnit->setText(m_query.value(3).toString());
  m_ui->editEkp->setText(m_query.value(4).toString());
  m_ui->editNetto->setText(m_query.value(5).toString());
  m_ui->editMinutes->setText(m_query.value(6).toString());
  m_ui->editBrutto->setText(m_query.value(7).toString());
  m_ui->labelTotal->setText(m_query.value(8).toString());
}

void MaterialPage::SetFocusToFirst()
{
  m_ui->editKey->setFocus();
}