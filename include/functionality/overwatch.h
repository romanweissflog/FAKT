#ifndef OVERWATCH_H
#define OVERWATCH_H

#include "utils.h"
#include "tabs\basetab.h"

#include <map>
#include <string>


class Overwatch
{
public:
  Overwatch(Overwatch const &other) = delete;
  Overwatch operator=(Overwatch const &other) = delete;

  static Overwatch& GetInstance()
  {
    static Overwatch instance;
    return instance;
  }

  void AddSubject(TabName name, BaseTab *tab)
  {
    m_tabs.emplace(name, tab);
  }

  BaseTab* GetTabPointer(TabName name) const
  {
    if (m_tabs.count(name) != 0)
    {
      return m_tabs.at(name);
    }
    return nullptr;
  }

private:
  Overwatch() {}

private:
  std::map<TabName, BaseTab*> m_tabs;
};

#endif