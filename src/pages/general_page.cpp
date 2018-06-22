#include "pages\general_page.h"
#include "functionality\overwatch.h"

#include "QtSql\qsqlerror.h"
#include "QtCore\qdebug.h"
#include "QtSql\qsqlquerymodel.h"

#include "QtCore\qdebug.h"
#include "QtSql\qsqlerror.h"
#include "QtWidgets\qshortcut.h"

#include "ui_general_content.h"
#include "ui_page_framework.h"


GeneralContent::GeneralContent(Settings *settings,
  uint64_t number,
  std::string const &child,
  std::optional<double> const &hourlyRate,
  QString const &key, 
  QWidget *parent)
  : ParentPage("GeneralPage", parent)
  , m_ui(new Ui::generalContent)
  , m_query(*Overwatch::GetInstance().GetDatabase())
  , m_hourlyRate(hourlyRate ? *hourlyRate : settings->hourlyRate)
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
  connect(new QShortcut(QKeySequence(Qt::Key_F5), this), &QShortcut::activated, [this]()
  {
    Overwatch::GetInstance().GetTabPointer(TabName::MaterialTab)->AddEntry(data);
  });
  connect(new QShortcut(QKeySequence(Qt::Key_F6), this), &QShortcut::activated, [this]()
  {
    Overwatch::GetInstance().GetTabPointer(TabName::ServiceTab)->AddEntry(data);
  });

  data = {};
  SetConnections();
  m_ui->labelPosError->setText(QString::fromStdString("Muss ausgef" + german::ue + "llt sein"));
  if (key.size() > 0)
  {
    m_ui->editPos->setText(key);
  }
  m_ui->editServiceRate->setText(l.toString(m_hourlyRate, 'f', 2));
  m_ui->editPos->setFocus();
}

void GeneralContent::CopyData(GeneralData *copyData)
{
  QLocale l(QLocale::German);

  m_ui->editPos->setText(copyData->pos);
  m_ui->editArtNr->setText(copyData->artNr);
  m_ui->editMainText->setText(copyData->mainText);
  m_ui->editText->setText(copyData->text);
  m_ui->editUnitSize->setText(QString::number(copyData->number));
  m_ui->editUnitType->setText(copyData->unit);
  m_ui->editMaterialEKP->setText(l.toString(copyData->ekp, 'f', 2));
  m_ui->editMaterialSurchage->setText(l.toString(copyData->surcharge, 'f', 2));
  m_ui->editMaterialPrice->setText(l.toString(copyData->material));
  m_ui->editMaterialDiscount->setText(l.toString(copyData->discount, 'f', 2));
  m_ui->editServiceTime->setText(l.toString(copyData->time, 'f', 2));
  m_ui->editServiceRate->setText(l.toString(copyData->hourlyRate, 'f', 2));
  m_ui->editServicePrice->setText(l.toString(copyData->service, 'f', 2));
  m_ui->editHelpMat->setText(l.toString(copyData->helpMat, 'f', 2));
  m_ui->labelEP->setText(l.toString(copyData->ep, 'f', 2));
  m_ui->labelPriceTotal->setText(l.toString(copyData->total, 'f', 2));
}

GeneralContent::~GeneralContent()
{}

void GeneralContent::SetFocusToFirst()
{
  m_ui->editPos->setFocus();
}

void GeneralContent::SetConnections()
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
    data.text = m_ui->editText->toPlainText();
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
    data.material = l.toDouble(txt);
    //double surchage = (data.ekp == 0 ? 0.0 : (txt.toDouble() - data.ekp) / data.ekp * 100);
    Calculate();
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
    //double time = l.toDouble(txt) * 60.0 / data.hourlyRate;
    //m_ui->editServiceTime->setText(l.toString(time, 'f', 2));
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data.helpMat = l.toDouble(txt);
    Calculate();
  });
}

void GeneralContent::Calculate()
{
  QLocale l(QLocale::German);
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

void GeneralContent::TakeFromMaterial()
{
  QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM MATERIAL";
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<QString> keys, mainDescription, descriptions;
  while (m_query.next())
  {
    keys.push_back(m_query.value(0).toString());
    mainDescription.push_back(m_query.value(1).toString());
    descriptions.push_back(m_query.value(2).toString());
  }
  int lastPos = -1;
  if (lastMaterialImport)
  {
    auto it = std::find(std::begin(keys), std::end(keys), *lastServiceImport);
    if (it != std::end(keys))
    {
      lastPos = std::distance(std::begin(keys), it);
    }
  }
  importPage = new CustomTable("Material-Import", keys.size(), { "Artikelnummer", "Kurzbeschreibung", "Beschreibung" }, this);
  importPage->SetColumn(0, keys);
  importPage->SetColumn(1, mainDescription);
  importPage->SetColumn(2, descriptions);
  importPage->SetSortingEnabled(lastPos);
  emit AddPage();
  connect(importPage, &CustomTable::SetSelected, [this](QString const &key)
  {
    lastMaterialImport.emplace(key);
    CopyMaterialData(key);
    emit ClosePage();
  });
  connect(importPage, &CustomTable::Close, [this]()
  {
    emit ClosePage();
  });
}

void GeneralContent::TakeFromService()
{
  QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM LEISTUNG";
  auto rc = m_query.exec(sql);
  if (!rc)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, m_query.lastError().text().toStdString());
    return;
  }
  std::vector<QString> keys, mainDescription, descriptions;
  while (m_query.next())
  {
    keys.push_back(m_query.value(0).toString());
    mainDescription.push_back(m_query.value(1).toString());
    descriptions.push_back(m_query.value(2).toString());
  }
  int lastPos = -1;
  if (lastServiceImport)
  {
    auto it = std::find(std::begin(keys), std::end(keys), *lastServiceImport);
    if (it != std::end(keys))
    {
      lastPos = std::distance(std::begin(keys), it);
    }
  }
  importPage = new CustomTable("Leistung-Import", keys.size(), { "Artikelnummer", "Kurzbeschreibung", "Beschreibung" }, this);
  importPage->SetColumn(0, keys);
  importPage->SetColumn(1, mainDescription);
  importPage->SetColumn(2, descriptions);
  importPage->SetSortingEnabled(lastPos);
  emit AddPage();
  connect(importPage, &CustomTable::SetSelected, [this](QString const &key)
  {
    lastServiceImport.emplace(key);
    CopyServiceData(key);
    emit ClosePage();
  });
  connect(importPage, &CustomTable::Close, [this]()
  {
    emit ClosePage();
  });
}

void GeneralContent::CopyMaterialData(QString const &key)
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::MaterialTab);
  if (tab == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Material tab not found in Overwatch");
    return;
  }

  auto input = tab->GetData(key.toStdString());
  std::unique_ptr<MaterialData> data(static_cast<MaterialData*>(input.release()));
  if (data == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Material data not found for article number " + key.toStdString());
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

void GeneralContent::CopyServiceData(QString const &key)
{
  Overwatch &tabs = Overwatch::GetInstance();
  auto tab = tabs.GetTabPointer(TabName::ServiceTab);
  if (tab == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Service tab not found in Overwatch");
    return;
  }

  auto input = tab->GetData(key.toStdString());
  std::unique_ptr<ServiceData> data(static_cast<ServiceData*>(input.release()));
  if (data == nullptr)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, "Service data not found for article number " + key.toStdString());
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


GeneralPage::GeneralPage(Settings *settings,
  uint64_t number,
  std::string const &child,
  std::optional<double> const &hourlyRate,
  QString const &key,
  QWidget *parent)
  : PageFramework(parent)
  , content(new GeneralContent(settings, number, child, hourlyRate, key, this))
{
  m_ui->mainLayout->replaceWidget(m_ui->content, content);

  content->setFocus();
  content->SetFocusToFirst();

  connect(content, &GeneralContent::AddPage, [this]()
  {
    emit AddExtraPage(content->importPage, "Import");
  });
  connect(content, &GeneralContent::ClosePage, [this]()
  {
    emit CloseExtraPage("Import");
    content->setFocus();
  });
}

GeneralPage::~GeneralPage()
{}

std::optional<QString> GeneralPage::GetLastMaterialImportKey() const
{
  return content->lastMaterialImport;
}

std::optional<QString> GeneralPage::GetLastServiceImportKey() const
{
  return content->lastServiceImport;
}

void GeneralPage::SetLastMaterialImportKey(std::optional<QString> const &key)
{
  content->lastMaterialImport = key;
}

void GeneralPage::SetLastServiceImportKey(std::optional<QString> const &key)
{
  content->lastServiceImport = key;
}
