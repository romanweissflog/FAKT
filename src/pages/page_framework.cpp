#include "pages\page_framework.h"
#include "functionality\overwatch.h"

#include "ui_page_framework.h"

#include "QtGui\qevent.h"

PageFramework::PageFramework(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::pageFramework)
{
  m_ui->setupUi(this);

  connect(m_ui->accept, &QPushButton::clicked, [this]()
  {
    emit Accepted();
  });
  connect(m_ui->decline, &QPushButton::clicked, [this]()
  {
    emit Declined();
  });
  m_ui->accept->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  m_ui->decline->installEventFilter(Overwatch::GetInstance().GetEventLogger());
  this->installEventFilter(Overwatch::GetInstance().GetEventLogger());
}

PageFramework::~PageFramework()
{}

void PageFramework::keyPressEvent(QKeyEvent *ev)
{
  if (ev->key() == Qt::Key_Escape)
  {
    emit Declined();
  }
  QWidget::keyPressEvent(ev);
}