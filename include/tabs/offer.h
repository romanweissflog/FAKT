#ifndef OFFER_H
#define OFFER_H

#include "functionality\utils.h"
#include "basetab.h"

#include "QtWidgets\qwidget.h"
#include "QtSql\qsqldatabase.h"
#include "QtSql\qsqlquery.h"

#include "QtWidgets\qlineedit.h"

#include <vector>
#include <cstdint>

class Offer : public BaseTab
{
  Q_OBJECT
public:
  Offer(QWidget *parent = nullptr);
  ~Offer();

  std::unique_ptr<Data> GetData(std::string const &artNr) override;

  void SetData(std::unique_ptr<Data> &data) override;

public slots:
  void AddEntry() override;
  void DeleteEntry() override;
  void EditEntry() override;

private:
  ReturnValue PrepareDoc(bool withLogo) override;
};

#endif
