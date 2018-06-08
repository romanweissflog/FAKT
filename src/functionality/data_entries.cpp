#include "functionality/data_entries.h"

SingleData& DatabaseData::operator [](QString const &key)
{
  auto it = std::find_if(std::begin(data), std::end(data), [key](std::pair<QString, SingleData> const &d)
  {
    return key == d.first;
  });
  if (it == std::end(data))
  {
    data.push_back(std::make_pair(key, SingleData{}));
    return data.back().second;
  }
  return it->second;
}

SingleData DatabaseData::operator [](QString const &key) const
{
  auto it = std::find_if(std::begin(data), std::end(data), [key](std::pair<QString, SingleData> const &d)
  {
    return key == d.first;
  });
  if (it == std::end(data))
  {
    throw std::runtime_error("Tried to access an element that does not exists in const call");
  }
  return it->second;
}