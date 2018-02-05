#include "pages\parent_page.h"

ParentPage::ParentPage(std::string const &childType, QWidget *parent)
  : QDialog(parent)
  , m_logId(Log::GetLog().RegisterInstance(childType))
{}

void ParentPage::keyPressEvent(QKeyEvent *ev)
{
  if ((ev->key() == Qt::Key_Enter && ev->modifiers() == Qt::Modifier::SHIFT)
    || (ev->key() == Qt::Key_Return && ev->modifiers() == Qt::Modifier::SHIFT))
  {
    focusPreviousChild();
    return;
  }
  if (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return)
  {
    focusNextChild();
    return;
  }
  QDialog::keyPressEvent(ev);
}

void ParentPage::SetData(Data *data)
{
  throw std::runtime_error("Set data not implemented yet for chosen type");
}
