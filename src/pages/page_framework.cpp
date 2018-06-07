#include "pages\page_framework.h"

#include "ui_page_framework.h"

#include "QtGui\qevent.h"

PageFramework::PageFramework(QWidget *parent)
  : QWidget(parent)
  , m_ui(new Ui::pageFramework)
{
  m_ui->setupUi(this);

  connect(m_ui->accept, &QPushButton::clicked, [this]()
  {
    HandleBeforeAccept();
    emit Accepted();
  });
  connect(m_ui->decline, &QPushButton::clicked, [this]()
  {
    emit Declined();
  });
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