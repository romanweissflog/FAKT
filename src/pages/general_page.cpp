#include "pages\general_page.h"
#include "functionality\overwatch.h"

#include "QtSql\qsqlerror.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlquerymodel.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qshortcut.h"

#include "ui_general_page.h"

GeneralPage::GeneralPage(Settings *settings,
  uint64_t number,
  std::string const &lastPos,
  QSqlQuery &query,
  QWidget *parent)
  : ParentPage("GeneralPage", parent)
  , m_ui(new Ui::generalPage)
  , m_query(query)
  , m_hourlyRate(settings->hourlyRate)
{
  m_ui->setupUi(this);
  m_ui->editText->setTabChangesFocus(true);
  QSqlQueryModel *model = new QSqlQueryModel(this);
  model->setQuery(m_query);

  m_ui->labelNr->setText(QString::number(number));
  m_ui->editPos->setText(QString::fromStdString(lastPos));
  m_ui->labelGenRate->setText(QString::number(m_hourlyRate));

  new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(TakeFromMaterial()));
  new QShortcut(QKeySequence(Qt::Key_F2), this, SLOT(TakeFromService()));
  new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(MakeNewEntry()));

  data = {};
  SetConnections();
}

void GeneralPage::CopyData(uint64_t number, std::string const &pos)
{
  if (!m_query.prepare("SELECT * FROM " + QString::number(number) + " WHERE POSIT = :ID"))
  {
    qDebug() << m_query.lastError();
  }
  m_query.bindValue(":ID", QString::fromStdString(pos));
  if (!m_query.exec())
  {
    qDebug() << m_query.lastError();
  }
  m_query.next();
  m_ui->editArtNr->setText(m_query.value(1).toString());
  m_ui->editText->setText(m_query.value(2).toString());
  m_ui->editUnitType->setText(m_query.value(3).toString());
  m_ui->editUnitSize->setText(m_query.value(4).toString());
  m_ui->labelEP->setText(m_query.value(5).toString());
  m_ui->editServicePrice->setText(m_query.value(6).toString());
  m_ui->editHelpMat->setText(m_query.value(7).toString());
  m_ui->labelPriceTotal->setText(m_query.value(8).toString());
  m_ui->editServiceTime->setText(m_query.value(9).toString());
  m_ui->editMaterialDiscount->setText(m_query.value(14).toString());
  m_ui->editArtNr->setText(m_query.value(16).toString());
  m_ui->editServiceRate->setText(m_query.value(17).toString());
  m_ui->editMaterialEKP->setText(m_query.value(18).toString());
}

GeneralPage::~GeneralPage()
{}

void GeneralPage::SetConnections()
{
  connect(m_ui->editPos, &QLineEdit::textChanged, [this](QString txt)
  {
    data.pos = txt;
  });
  connect(m_ui->editArtNr, &QLineEdit::textChanged, [this](QString txt)
  {
    data.artNr = txt;
  });
  connect(m_ui->editText, &QTextEdit::textChanged, [this]()
  {
    data.text = m_ui->editText->toPlainText();;
  });
  connect(m_ui->editUnitType, &QLineEdit::textChanged, [this](QString txt)
  {
    data.unit = txt;
  });
  connect(m_ui->editUnitSize, &QLineEdit::textChanged, [this](QString txt)
  {
    data.number = txt.toULong();
    Calculate();
  });
  connect(m_ui->editMaterialEKP, &QLineEdit::textChanged, [this](QString txt)
  {
    data.ekp = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editMaterialSurchage, &QLineEdit::textChanged, [this](QString txt)
  {
    data.surcharge = txt.toDouble();
    double matPrice = (100.0 + data.surcharge) / 100.0 * data.ekp;
    m_ui->editMaterialPrice->setText(QString::number(matPrice));
  });
  connect(m_ui->editMaterialPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.material = txt.toDouble();
    double surchage = (data.ekp == 0 ? -100.0 : (data.material - data.ekp) / data.ekp * 100);
    m_ui->editMaterialSurchage->setText(QString::number(surchage));
    Calculate();
  });
  connect(m_ui->editMaterialDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    data.discount = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServiceRate, &QLineEdit::textChanged, [this](QString txt)
  {
    data.hourlyRate = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServiceTime, &QLineEdit::textChanged, [this](QString txt)
  {
    data.time = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editServicePrice, &QLineEdit::textChanged, [this](QString txt)
  {
    data.service = txt.toDouble();
    Calculate();
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helpMat = txt.toDouble();
    Calculate();
  });
}

void GeneralPage::Calculate()
{
  double profitMatPerc = (data.ekp == 0 ? 100.0 : (data.material - data.ekp) / data.material * 100);
  m_ui->labelProfitMatPerc->setText(QString::number(profitMatPerc));

  double profitMatEur = (data.material - data.ekp) * data.number;
  m_ui->labelProfitMatEur->setText(QString::number(profitMatEur));
  m_ui->labelWorkingHours->setText(QString::number(data.number*data.time));

  double matPrice = (100.0 - data.discount) / 100.0 * data.material;
  m_ui->labelMaterialQuant->setText(QString::number(matPrice));

  double servicePrice = data.time / 60.0*data.hourlyRate;
  m_ui->editServicePrice->setText(QString::number(servicePrice));

  double ep = matPrice + servicePrice + data.helpMat;
  m_ui->labelEP->setText(QString::number(ep));
  data.ep = ep;
  m_ui->labelPriceTotal->setText(QString::number(ep*data.number));
  data.total = ep*data.number;
}

void GeneralPage::TakeFromMaterial()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::MaterialTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto data = static_cast<MaterialData*>(tab->GetData(chosenArtNr.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Material data not found");
    }
    m_ui->editArtNr->setText(data->key);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(QString::number(data->ekp));
    m_ui->editMaterialPrice->setText(QString::number(data->netto));
  }
}

void GeneralPage::TakeFromService()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::ServiceTab);
  if (tab == nullptr)
  {
    throw std::runtime_error("Tab not found in overwatch");
  }

  auto artNumbers = tab->GetArtNumbers();
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto data = static_cast<ServiceData*>(tab->GetData(chosenArtNr.toStdString()));
    if (data == nullptr)
    {
      throw std::runtime_error("Material data not found");
    }
    m_ui->editArtNr->setText(data->key);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(QString::number(data->ekp));
    m_ui->editMaterialPrice->setText(QString::number(data->material));
    m_ui->editServiceTime->setText(QString::number(data->minutes));
    m_ui->editServicePrice->setText(QString::number(data->service));
    m_ui->editHelpMat->setText(QString::number(data->helperMaterial));
  }
}

void GeneralPage::MakeNewEntry()
{

}
