#include "pages\parent_page.h"

ParentPage::ParentPage(std::string const &childType, QWidget *parent)
  : QWidget(parent)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{}

void ParentPage::keyPressEvent(QKeyEvent *ev)
{
  if ((ev->modifiers() == Qt::Modifier::ALT)
    || (ev->modifiers() == Qt::KeyboardModifier::AltModifier + Qt::KeyboardModifier::KeypadModifier))
  {
    return;
  }
  if ((ev->modifiers() == Qt::Modifier::CTRL && ev->key() == Qt::Key_Return)
    || (ev->modifiers() == Qt::KeyboardModifier::ControlModifier + Qt::KeyboardModifier::KeypadModifier && ev->key() == Qt::Key_Enter))
  {
    QWidget::keyPressEvent(ev);
    return;
  }
  if ((ev->modifiers() == Qt::Modifier::SHIFT && ev->key() == Qt::Key_Return)
    || (ev->modifiers() == Qt::KeyboardModifier::ShiftModifier + Qt::KeyboardModifier::KeypadModifier && ev->key() == Qt::Key_Enter))
  {
    focusPreviousChild();
    return;
  }
  if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
  {
    auto x = ev->modifiers();
    focusNextChild();
    return;
  }
  QWidget::keyPressEvent(ev);
}

void ParentPage::SetData(Data *data)
{
  Log::GetLog().Write(LogType::LogTypeError, m_logId, "Set data not implemented for chosen type");
}
