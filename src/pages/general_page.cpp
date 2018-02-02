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
  std::string const &child,
  QSqlQuery &query,
  QWidget *parent)
  : ParentPage("GeneralPage", parent)
  , m_ui(new Ui::generalPage)
  , m_query(query)
  , m_hourlyRate(settings->hourlyRate)
{
  m_ui->setupUi(this);
  m_ui->labelNumberType->setText(QString::fromStdString(child) + " - Nummer:");
  m_ui->labelTotalType->setText(QString::fromStdString(child) + " gesamt:");
  m_ui->editText->setTabChangesFocus(true);
  QSqlQueryModel *model = new QSqlQueryModel(this);
  model->setQuery(m_query);

  m_ui->labelNr->setText(QString::number(number));
  m_ui->labelGenRate->setText(QString::number(m_hourlyRate));

  new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(TakeFromMaterial()));
  new QShortcut(QKeySequence(Qt::Key_F2), this, SLOT(TakeFromService()));
  new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(MakeNewEntry()));

  data = {};
  SetConnections();
  m_ui->editServiceRate->setText(QString::number(m_hourlyRate));
  m_ui->labelPosError->setText(QString::fromStdString("Muss ausgef" + german::ue + "llt sein"));
}

void GeneralPage::CopyData(GeneralData *data)
{
  m_ui->editPos->setText(data->pos);
  m_ui->editArtNr->setText(data->artNr);
  m_ui->editText->setText(data->text);
  m_ui->editUnitSize->setText(QString::number(data->number));
  m_ui->editUnitType->setText(data->unit);
  m_ui->editMaterialDiscount->setText(QString::number(data->discount));
  m_ui->editMaterialEKP->setText(QString::number(data->ekp));
  m_ui->editMaterialSurchage->setText(QString::number(data->surcharge));
  m_ui->labelMaterialQuant->setText(QString::number(data->material));
  m_ui->editServiceTime->setText(QString::number(data->time));
  m_ui->editServiceRate->setText(QString::number(data->hourlyRate));
  m_ui->editServicePrice->setText(QString::number(data->service));
  m_ui->editHelpMat->setText(QString::number(data->helpMat));
  m_ui->labelEP->setText(QString::number(data->ep));
  m_ui->labelPriceTotal->setText(QString::number(data->total));
}

GeneralPage::~GeneralPage()
{}

void GeneralPage::SetConnections()
{
  connect(m_ui->editPos, &QLineEdit::textChanged, [this](QString txt)
  {
    data.pos = txt;
    if (txt.size() == 0)
    {
      m_ui->labelPosError->setText(QString::fromStdString("Muss ausgef" + german::ue + "llt sein"));
    }
    else
    {
      m_ui->labelPosError->setText("");
    }
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
    double matPriceSurcharge = (100.0 + data.surcharge) / 100.0 * data.ekp;
    m_ui->editMaterialPrice->setText(QString::number(matPriceSurcharge));
    Calculate();
  });
  connect(m_ui->editMaterialPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    double surchage = (data.ekp == 0 ? 0.0 : (txt.toDouble() - data.ekp) / data.ekp * 100);
    m_ui->editMaterialSurchage->setText(QString::number(surchage));
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
    double time = txt.toDouble() * 60.0 / data.hourlyRate;
    m_ui->editServiceTime->setText(QString::number(time));
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    data.helpMat = txt.toDouble();
    Calculate();
  });
}

void GeneralPage::Calculate()
{
  double matPriceSurcharge = (100.0 + data.surcharge) / 100.0 * data.ekp;
  data.material = (100.0 - data.discount) / 100.0 * matPriceSurcharge;
  m_ui->labelMaterialQuant->setText(QString::number(data.material));

  double profitMatPerc = (data.ekp == 0 ? 100.0 : (data.material - data.ekp) / data.material * 100);
  m_ui->labelProfitMatPerc->setText(QString::number(profitMatPerc));
  double profitMatEur = (data.material - data.ekp) * data.number;
  m_ui->labelProfitMatEur->setText(QString::number(profitMatEur));

  double servicePrice = data.time / 60.0 * data.hourlyRate;
  m_ui->labelWorkingHours->setText(QString::number(data.number * data.time));
  m_ui->editServicePrice->setText(QString::number(servicePrice));

  double ep = data.material + data.service + data.helpMat;
  m_ui->labelEP->setText(QString::number(ep));
  data.ep = ep;

  data.total = ep * data.number;
  m_ui->labelPriceTotal->setText(QString::number(data.total));
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
    auto input = tab->GetData(chosenArtNr.toStdString());
    std::unique_ptr<MaterialData> data(static_cast<MaterialData*>(input.release()));
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
    auto input = tab->GetData(chosenArtNr.toStdString());
    std::unique_ptr<ServiceData> data(static_cast<ServiceData*>(input.release()));
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
  MaterialOrService *page = new MaterialOrService(this);
  if (page->exec() == QDialog::Accepted)
  {
    TabName chosen = page->chosenTab;
    if (chosen != TabName::UndefTab)
    {
      Overwatch::GetInstance().GetTabPointer(chosen)->AddEntry();
    }
  }
}
