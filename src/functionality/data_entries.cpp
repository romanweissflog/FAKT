#include "functionality/data_entries.h"

SingleData& DatabaseData::operator [](QString const &key)
{
  auto it = std::find_if(std::begin(data), std::end(data), [key](DatabaseDataPair const &d)
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
  auto it = std::find_if(std::begin(data), std::end(data), [key](DatabaseDataPair const &d)
  {
    return key.compare(d.first) == 0;
  });
  if (it == std::end(data))
  {
    throw std::runtime_error("Tried to access " + key.toStdString() + " that does not exists in const call");
  }
  return it->second;
}

QString DatabaseData::GetString(QString const &key) const
{
  return this->operator[](key).entry.toString();
}

double DatabaseData::GetDouble(QString const &key) const
{
  return this->operator[](key).entry.toDouble();
}

double DatabaseData::GetDoubleIfAvailable(QString const &key) const
{
  auto it = std::find_if(std::begin(data), std::end(data), [key](DatabaseDataPair const &d)
  {
    return key.compare(d.first) == 0;
  });
  if (it == std::end(data))
  {
    return 0.0;
  }
  return it->second.entry.toDouble();
}
