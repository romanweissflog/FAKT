#include "adding_pages.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtGui\qevent.h"

#include "ui_leistung_page.h"

LeistungPage::LeistungPage(Settings *settings, QSqlQuery &query, QWidget *parent)
  : QDialog(parent)
  , m_ui(new Ui::leistungPage)
  , m_euroPerMin(settings->euroPerMin)
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
  connect(m_ui->editServicePeriod, &QLineEdit::textChanged, [this](QString txt)
  {
    data.minutes = txt.toDouble();
    double price = data.minutes*m_euroPerMin;
    m_ui->labelServicePrice->setText(QString::number(price));
    Calculate();
  });
  connect(m_ui->editMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.material = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editHelperMatPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helperMaterial = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMatEkp, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
  });

  m_ui->copyBox->addItem("");
  if (m_query.exec("SELECT ARTNR FROM LEISTUNG"))
  {
    qDebug() << m_query.lastError();
  }
  while (m_query.next())
  {
    m_ui->copyBox->addItem(m_query.value(0).toString());
  }
}

LeistungPage::~LeistungPage()
{}

void LeistungPage::keyPressEvent(QKeyEvent *ev)
{
  if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
    return;
  QDialog::keyPressEvent(ev);
}

void LeistungPage::Calculate()
{
  double value = data.service + data.material + data.helperMaterial;
  m_ui->labelTotal->setText(QString::number(value));
  data.ep = value;
}

void LeistungPage::CopyData(QString txt)
{
  if (!m_query.prepare("SELECT * FROM LEISTUNG WHERE ARTNR = :ID"))
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
  m_ui->editMatPrice->setText(m_query.value(3).toString());
  m_ui->editServicePeriod->setText(m_query.value(4).toString());
  m_ui->labelServicePrice->setText(m_query.value(5).toString());
  m_ui->editHelperMatPrice->setText(m_query.value(6).toString());
  m_ui->labelTotal->setText(m_query.value(7).toString());
  m_ui->editUnit->setText(m_query.value(8).toString());
  m_ui->editMatEkp->setText(m_query.value(9).toString());
}