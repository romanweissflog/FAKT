#include "tabs/main_tab.h"
#include "functionality\overwatch.h"

#include "QtWidgets\qshortcut.h"

#include "ui_main_tab.h"

MainTab::MainTab(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::main_tab)
{
  m_ui->setupUi(this);
  connect(m_ui->material, &QPushButton::clicked, [this]()
  {
    emit AddTab(0);
  });
  connect(m_ui->service, &QPushButton::clicked, [this]()
  {
    emit AddTab(1);
  });
  connect(m_ui->address, &QPushButton::clicked, [this]()
  {
    emit AddTab(2);
  });
  connect(m_ui->offer, &QPushButton::clicked, [this]()
  {
    emit AddTab(3);
  });
  connect(m_ui->jobsite, &QPushButton::clicked, [this]()
  {
    emit AddTab(4);
  });
  connect(m_ui->invoice, &QPushButton::clicked, [this]()
  {
    emit AddTab(5);
  });
  m_ui->material->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->service->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->address->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->offer->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->jobsite->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->invoice->installEventFilter(Overwatch::GetInstance().GetEventLogger());

  for (int k = Qt::Key_A, i = 0; i < 6; ++k, ++i)
  {
    QString name = "";
    switch (k)
    {
    case Qt::Key_A: name = "Key_A"; break;
    case Qt::Key_B: name = "Key_B"; break;
    case Qt::Key_C: name = "Key_C"; break;
    case Qt::Key_D: name = "Key_D"; break;
    case Qt::Key_E: name = "Key_E"; break;
    case Qt::Key_F: name = "Key_F"; break;
    default: name = "Unknown";
    }
    QShortcut *shortcut = new QShortcut(QKeySequence(k), this);
    shortcut->setObjectName(name);
    shortcut->installEventFilter(Overwatch::GetInstance().GetEventLogger());
    connect(shortcut, &QShortcut::activated, [this, i]()
    {
      emit AddTab(i);
    });
  }
}

MainTab::~MainTab()
{}
