#include "pages\parent_page.h"

ParentPage::ParentPage(std::string const &childType, QWidget *parent)
  : QWidget(parent)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{}

void ParentPage::keyPressEvent(QKeyEvent *ev)
{
  QWidget::keyPressEvent(ev);
}

void ParentPage::SetData(Data *data)
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "Set data not implemented for chosen type");
}
