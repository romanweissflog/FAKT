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
  QSqlQueryModel *model = new QSqlQueryModel(this);
  model->setQuery(m_query);

  QLocale l(QLocale::German);

  m_ui->labelNr->setText(QString::number(number));
  m_ui->labelGenRate->setText(l.toString(m_hourlyRate, 'f', 2));

  new QShortcut(QKeySequence(Qt::Key_F1), this, SLOT(TakeFromMaterial()));
  new QShortcut(QKeySequence(Qt::Key_F2), this, SLOT(TakeFromService()));
  new QShortcut(QKeySequence(Qt::Key_F5), this, SLOT(MakeNewEntry()));

  data = {};
  SetConnections();
  m_ui->editServiceRate->setText(l.toString(m_hourlyRate, 'f', 2));
  m_ui->labelPosError->setText(QString::fromStdString("Muss ausgef" + german::ue + "llt sein"));
  m_ui->editPos->setFocus();
}

void GeneralPage::CopyData(GeneralData *data)
{
  QLocale l(QLocale::German);

  m_ui->editPos->setText(data->pos);
  m_ui->editArtNr->setText(data->artNr);
  m_ui->editMainText->setText(data->mainText);
  m_ui->editText->setText(data->text);
  m_ui->editUnitSize->setText(QString::number(data->number));
  m_ui->editUnitType->setText(data->unit);
  m_ui->editMaterialDiscount->setText(l.toString(data->discount, 'f', 2));
  m_ui->editMaterialEKP->setText(l.toString(data->ekp, 'f', 2));
  m_ui->editMaterialSurchage->setText(l.toString(data->surcharge, 'f', 2));
  m_ui->labelMaterialQuant->setText(l.toString(data->material, 'f', 2));
  m_ui->editServiceTime->setText(l.toString(data->time, 'f', 2));
  m_ui->editServiceRate->setText(l.toString(data->hourlyRate, 'f', 2));
  m_ui->editServicePrice->setText(l.toString(data->service, 'f', 2));
  m_ui->editHelpMat->setText(l.toString(data->helpMat, 'f', 2));
  m_ui->labelEP->setText(l.toString(data->ep, 'f', 2));
  m_ui->labelPriceTotal->setText(l.toString(data->total, 'f', 2));
}

GeneralPage::~GeneralPage()
{}

void GeneralPage::SetFocusToFirst()
{
  m_ui->editPos->setFocus();
}

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
  connect(m_ui->editMainText, &QLineEdit::textChanged, [this](QString txt)
  {
    data.mainText = txt;
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
    QLocale l(QLocale::German);
    data.ekp = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editMaterialSurchage, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.surcharge = l.toDouble(txt);
    double matPriceSurcharge = (100.0 + data.surcharge) / 100.0 * data.ekp;
    m_ui->editMaterialPrice->setText(l.toString(matPriceSurcharge, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMaterialPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double surchage = (data.ekp == 0 ? 0.0 : (txt.toDouble() - data.ekp) / data.ekp * 100);
    Calculate();
    //m_ui->editMaterialSurchage->setText(l.toString(surchage, 'f', 2));
  });
  connect(m_ui->editMaterialDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.discount = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editServiceRate, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.hourlyRate = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editServiceTime, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.time = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editServicePrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.service = l.toDouble(txt);
    double time = l.toDouble(txt) * 60.0 / data.hourlyRate;
    m_ui->editServiceTime->setText(l.toString(time, 'f', 2));
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.helpMat = l.toDouble(txt);
    Calculate();
  });
}

void GeneralPage::Calculate()
{
  QLocale l(QLocale::German);
  double matPriceSurcharge = (100.0 + data.surcharge) / 100.0 * data.ekp;
  data.material = (100.0 - data.discount) / 100.0 * matPriceSurcharge;
  m_ui->labelMaterialQuant->setText(l.toString(data.material, 'f', 2));

  double profitMatPerc = (data.ekp == 0 ? 100.0 : (data.material - data.ekp) / data.material * 100);
  m_ui->labelProfitMatPerc->setText(l.toString(profitMatPerc, 'f', 2));
  double profitMatEur = (data.material - data.ekp) * data.number;
  m_ui->labelProfitMatEur->setText(l.toString(profitMatEur, 'f', 2));

  double servicePrice = data.time / 60.0 * data.hourlyRate;
  m_ui->labelWorkingHours->setText(l.toString(data.number * data.time, 'f', 2));
  m_ui->editServicePrice->setText(l.toString(servicePrice, 'f', 2));

  double ep = data.material + data.service + data.helpMat;
  m_ui->labelEP->setText(l.toString(ep, 'f', 2));
  data.ep = ep;

  data.total = ep * data.number;
  m_ui->labelPriceTotal->setText(l.toString(data.total, 'f', 2));
}

void GeneralPage::TakeFromMaterial()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::MaterialTab);
  if (tab == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Material tab not found in Overwatch");
    return;
  }

  auto artNumbers = tab->GetRowData("ARTNR");
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto input = tab->GetData(chosenArtNr.toStdString());
    std::unique_ptr<MaterialData> data(static_cast<MaterialData*>(input.release()));
    if (data == nullptr)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, "Material data not found for article number " + chosenArtNr.toStdString());
      return;
    }
    QLocale l(QLocale::German);
    m_ui->editArtNr->setText(data->key);
    m_ui->editMainText->setText(data->mainDescription);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(l.toString(data->ekp, 'f', 2));
    m_ui->editMaterialPrice->setText(l.toString(data->netto, 'f', 2));
    m_ui->editServiceTime->setText(l.toString(data->minutes, 'f', 2));
  }
}

void GeneralPage::TakeFromService()
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::ServiceTab);
  if (tab == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Service tab not found in Overwatch");
    return;
  }

  auto artNumbers = tab->GetRowData("ARTNR");
  ShowValueList *dia = new ShowValueList(artNumbers, this);
  if (dia->exec() == QDialog::Accepted)
  {
    QString chosenArtNr = dia->currentItem;
    auto input = tab->GetData(chosenArtNr.toStdString());
    std::unique_ptr<ServiceData> data(static_cast<ServiceData*>(input.release()));
    if (data == nullptr)
    {
      Log::GetLog().Write(LogType::LogTypeError, m_logId, "Service data not found for article number " + chosenArtNr.toStdString());
      return;
    }
    QLocale l(QLocale::German);
    m_ui->editArtNr->setText(data->key);
    m_ui->editMainText->setText(data->mainDescription);
    m_ui->editText->setText(data->description);
    m_ui->editUnitType->setText(data->unit);
    m_ui->editMaterialEKP->setText(l.toString(data->ekp, 'f', 2));
    m_ui->editMaterialPrice->setText(l.toString(data->material, 'f', 2));
    m_ui->editServiceTime->setText(l.toString(data->minutes, 'f', 2));
    m_ui->editServicePrice->setText(l.toString(data->service, 'f', 2));
    m_ui->editHelpMat->setText(l.toString(data->helperMaterial, 'f', 2));
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
