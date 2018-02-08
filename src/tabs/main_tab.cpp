#include "tabs/main_tab.h"

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

  for (int k = Qt::Key_A, i = 0; i < 6; ++k, ++i)
  {
    connect(new QShortcut(QKeySequence(k), this), &QShortcut::activated, [this, i]()
    {
      emit AddTab(i);
    });
  }
}

MainTab::~MainTab()
{}
