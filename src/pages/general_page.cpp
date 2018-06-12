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
  QString const &key, 
  QWidget *parent)
  : ParentPage("GeneralPage", parent)
  , m_ui(new Ui::generalContent)
  , m_query(*Overwatch::GetInstance().GetDatabase())
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
  connect(new QShortcut(QKeySequence(Qt::Key_F5), this), &QShortcut::activated, [this]()
  {
    Overwatch::GetInstance().GetTabPointer(TabName::MaterialTab)->AddEntry();
  });
  connect(new QShortcut(QKeySequence(Qt::Key_F6), this), &QShortcut::activated, [this]()
  {
    Overwatch::GetInstance().GetTabPointer(TabName::ServiceTab)->AddEntry();
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

void GeneralContent::CopyData(DatabaseData const &copyData)
{
  QLocale l(QLocale::German);

  m_ui->editPos->setText(copyData.GetString("POSIT"));
  m_ui->editArtNr->setText(copyData.GetString("ARTNR"));
  m_ui->editMainText->setText(copyData.GetString("HAUPTARTBEZ"));
  m_ui->editText->setText(copyData.GetString("ARTBEZ"));
  m_ui->editUnitSize->setText(l.toString(copyData.GetDouble("MENGE"), 'f', 2));
  m_ui->editUnitType->setText(copyData.GetString("ME"));
  m_ui->editMaterialEKP->setText(l.toString(copyData.GetDouble("EKP"), 'f', 2));
  m_ui->editMaterialSurchage->setText(l.toString(copyData.GetDouble("MULTI"), 'f', 2));
  m_ui->editMaterialPrice->setText(l.toString(copyData.GetDouble("MP"), 'f', 2));
  m_ui->editMaterialDiscount->setText(l.toString(copyData.GetDouble("P_RABATT"), 'f', 2));
  m_ui->editServiceTime->setText(l.toString(copyData.GetDouble("BAUZEIT"), 'f', 2));
  m_ui->editServiceRate->setText(l.toString(copyData.GetDouble("STUSATZ"), 'f', 2));
  m_ui->editServicePrice->setText(l.toString(copyData.GetDouble("LP"), 'f', 2));
  m_ui->editHelpMat->setText(l.toString(copyData.GetDouble("SP"), 'f', 2));
  m_ui->labelEP->setText(l.toString(copyData.GetDouble("EP"), 'f', 2));
  m_ui->labelPriceTotal->setText(l.toString(copyData.GetDouble("GP"), 'f', 2));
}

void GeneralContent::SetFocusToFirst()
{
  m_ui->editPos->setFocus();
}

void GeneralContent::SetConnections()
{
  connect(m_ui->editPos, &QLineEdit::textChanged, [this](QString txt)
  {
    data["POSIT"].entry = txt;
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
    data["ARTNR"].entry = txt;
  });
  connect(m_ui->editMainText, &QLineEdit::textChanged, [this](QString txt)
  {
    data["HAUPTARTBEZ"].entry = txt;
  });
  connect(m_ui->editText, &QTextEdit::textChanged, [this]()
  {
    data["ARTBEZ"].entry = m_ui->editText->toPlainText();;
  });
  connect(m_ui->editUnitType, &QLineEdit::textChanged, [this](QString txt)
  {
    data["ME"].entry = txt;
  });
  connect(m_ui->editUnitSize, &QLineEdit::textChanged, [this](QString txt)
  {
    data["MENGE"].entry = txt.toInt();
    Calculate();
  });
  connect(m_ui->editMaterialEKP, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const value = l.toDouble(txt);
    data["EKP"].entry = value;
    double const mp = (100.0 + data.GetDoubleIfAvailable("MULTI")) / 100.0 * value;
    m_ui->editMaterialPrice->setText(l.toString(mp, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMaterialSurchage, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const value = l.toDouble(txt);
    data["MULTI"].entry = value;
    double const matPriceSurcharge = (100.0 + value) / 100.0 * data.GetDoubleIfAvailable("EKP");
    m_ui->editMaterialPrice->setText(l.toString(matPriceSurcharge, 'f', 2));
    Calculate();
  });
  connect(m_ui->editMaterialPrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["MP"].entry = l.toDouble(txt);
    //double surchage = (data.ekp == 0 ? 0.0 : (txt.toDouble() - data.ekp) / data.ekp * 100);
    Calculate();
  });
  connect(m_ui->editMaterialDiscount, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["P_RABATT"].entry = l.toDouble(txt);
    Calculate();
  });
  connect(m_ui->editServiceRate, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const value = l.toDouble(txt);
    data["STUSATZ"].entry = value;
    double const servicePrice = data.GetDoubleIfAvailable("BAUZEIT") / 60.0 * value;
    m_ui->editServicePrice->setText(l.toString(servicePrice, 'f', 2));
    Calculate();
  });
  connect(m_ui->editServiceTime, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    double const value = l.toDouble(txt);
    data["BAUZEIT"].entry = value;
    double const servicePrice = value / 60.0 * data.GetDoubleIfAvailable("STUSATZ");
    m_ui->editServicePrice->setText(l.toString(servicePrice, 'f', 2));
    Calculate();
  });
  connect(m_ui->editServicePrice, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["LP"].entry = l.toDouble(txt);
    Calculate();
    //double time = l.toDouble(txt) * 60.0 / data["STUSATZ"].entry.toDouble();
    //m_ui->editServiceTime->setText(l.toString(time, 'f', 2));
  });
  connect(m_ui->editHelpMat, &QLineEdit::textChanged, [this](QString txt)
  {
    QLocale l(QLocale::German);
    data["SP"].entry = l.toDouble(txt);
    Calculate();
  });
}

void GeneralContent::Calculate()
{
  try
  {
    double const material = data.GetDoubleIfAvailable("MP");
    double const service = data.GetDoubleIfAvailable("LP");
    double const ekp = data.GetDoubleIfAvailable("EKP");
    double const number = data.GetDoubleIfAvailable("MENGE");
    double const time = data.GetDoubleIfAvailable("BAUZEIT");
    double const helpMat = data.GetDoubleIfAvailable("SP");

    QLocale l(QLocale::German);
    m_ui->labelMaterialQuant->setText(l.toString(material, 'f', 2));

    double const profitMatPerc = (ekp == 0 ? 100.0 : (material - ekp) / material * 100);
    m_ui->labelProfitMatPerc->setText(l.toString(profitMatPerc, 'f', 2));
    double profitMatEur = (material - ekp) * number;
    m_ui->labelProfitMatEur->setText(l.toString(profitMatEur, 'f', 2));

    m_ui->labelWorkingHours->setText(l.toString(number * time, 'f', 2));

    double const ep = material + service + helpMat;
    m_ui->labelEP->setText(l.toString(ep, 'f', 2));
    data["EP"].entry = ep;

    double const total = ep * number;
    data["GP"].entry = total;
    m_ui->labelPriceTotal->setText(l.toString(total, 'f', 2));
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogTypeError, m_logId, e.what());
  }
}

void GeneralContent::TakeFromMaterial()
{
  try
  {
    QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM MATERIAL";
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
    }
    std::vector<QString> keys, mainDescription, descriptions;
    while (m_query.next())
    {
      keys.push_back(m_query.value(0).toString());
      mainDescription.push_back(m_query.value(1).toString());
      descriptions.push_back(m_query.value(2).toString());
    }
    importPage = new CustomTable("Material-Import", keys.size(), { "Artikelnummer", "Kurzbeschreibung", "Beschreibung" }, this);
    importPage->SetColumn(0, keys);
    importPage->SetColumn(1, mainDescription);
    importPage->SetColumn(2, descriptions);
    importPage->SetSortingEnabled();
    emit AddPage();
    connect(importPage, &CustomTable::SetSelected, [this](QString const &key)
    {
      CopyMaterialData(key);
      emit ClosePage();
    });
    connect(importPage, &CustomTable::Close, [this]()
    {
      emit ClosePage();
    });
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}

void GeneralContent::TakeFromService()
{
  try
  {
    QString sql = "SELECT ARTNR, HAUPTARTBEZ, ARTBEZ FROM LEISTUNG";
    auto rc = m_query.exec(sql);
    if (!rc)
    {
      throw std::runtime_error(m_query.lastError().text().toStdString());
    }
    std::vector<QString> keys, mainDescription, descriptions;
    while (m_query.next())
    {
      keys.push_back(m_query.value(0).toString());
      mainDescription.push_back(m_query.value(1).toString());
      descriptions.push_back(m_query.value(2).toString());
    }
    importPage = new CustomTable("Leistung-Import", keys.size(), { "Artikelnummer", "Kurzbeschreibung", "Beschreibung" }, this);
    importPage->SetColumn(0, keys);
    importPage->SetColumn(1, mainDescription);
    importPage->SetColumn(2, descriptions);
    importPage->SetSortingEnabled();
    emit AddPage();
    connect(importPage, &CustomTable::SetSelected, [this](QString const &key)
    {
      CopyServiceData(key);
      emit ClosePage();
    });
    connect(importPage, &CustomTable::Close, [this]()
    {
      emit ClosePage();
    });
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}

void GeneralContent::CopyMaterialData(QString const &key)
{
  try
  {
    Overwatch &tabs = Overwatch::GetInstance();
    auto tab = tabs.GetTabPointer(TabName::MaterialTab);
    if (tab == nullptr)
    {
      throw std::runtime_error("Material tab not found in Overwatch");
    }

    auto const data = tab->GetData(key.toStdString());

    QLocale l(QLocale::German);
    m_ui->editArtNr->setText(data.GetString("ARTNR"));
    m_ui->editMainText->setText(data.GetString("HAUPTARTBEZ"));
    m_ui->editText->setText(data.GetString("ARTBEZ"));
    m_ui->editUnitType->setText(data.GetString("ME"));
    m_ui->editMaterialEKP->setText(l.toString(data.GetDouble("EKP"), 'f', 2));
    m_ui->editMaterialPrice->setText(l.toString(data.GetDouble("NETTO"), 'f', 2));
    m_ui->editServiceTime->setText(l.toString(data.GetDouble("BAUZEIT"), 'f', 2));
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}

void GeneralContent::CopyServiceData(QString const &key)
{
  try
  {
    Overwatch &tabs = Overwatch::GetInstance();
    auto tab = tabs.GetTabPointer(TabName::ServiceTab);
    if (tab == nullptr)
    {
      throw std::runtime_error("Service tab not found in Overwatch");
    }

    auto const data = tab->GetData(key.toStdString());

    QLocale l(QLocale::German);
    m_ui->editArtNr->setText(data.GetString("ARTNR"));
    m_ui->editMainText->setText(data.GetString("HAUPTARTBEZ"));
    m_ui->editText->setText(data.GetString("ARTBEZ"));
    m_ui->editUnitType->setText(data.GetString("ME"));
    m_ui->editMaterialEKP->setText(l.toString(data.GetDouble("EKP"), 'f', 2));
    m_ui->editMaterialPrice->setText(l.toString(data.GetDouble("MP"), 'f', 2));
    m_ui->editServiceTime->setText(l.toString(data.GetDouble("BAUZEIT"), 'f', 2));
    m_ui->editServicePrice->setText(l.toString(data.GetDouble("LP"), 'f', 2));
    m_ui->editHelpMat->setText(l.toString(data.GetDouble("SP"), 'f', 2));
  }
  catch (std::runtime_error e)
  {
    Log::GetLog().Write(LogType::LogTypeError, m_logId, e.what());
  }
}


GeneralPage::GeneralPage(Settings *settings,
  uint64_t number,
  std::string const &child,
  QString const &key,
  QWidget *parent)
  : PageFramework(parent)
  , content(new GeneralContent(settings, number, child, key, this))
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

DatabaseData GeneralPage::GetData() const
{
  return content->data;
}
