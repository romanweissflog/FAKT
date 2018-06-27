#include "pages\parent_page.h"
#include "functionality\overwatch.h"

ParentPage::ParentPage(std::string const &childType, QWidget *parent)
  : QWidget(parent)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{
  this->installEventFilter(Overwatch::GetInstance().GetEventLogger());
}

void ParentPage::keyPressEvent(QKeyEvent *ev)
{
  QWidget::keyPressEvent(ev);
}

void ParentPage::SetData(Data *data)
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "Set data not implemented for chosen type");
}
