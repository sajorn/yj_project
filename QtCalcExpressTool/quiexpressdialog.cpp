#include "quiexpressdialog.h"
#include "qtcalcexpress.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
QuiExpressDialog::QuiExpressDialog()
{
  resize(300, 50);
  InitUi();
  InitConnect();
}

void QuiExpressDialog::SlotOnCalc()
{
  QString strExpress = m_pExpressEdit->text();
  if (strExpress.isEmpty())
  {
    return;
  }

  if (!strExpress.back().isDigit())
  {
    QMessageBox::critical(this, "Error", "Illegal expression");
    return;
  }

  QtCalcExpress calcExpress(strExpress);
  int nResult = calcExpress.CalcExpress();
  m_pResultLabel->setText(QString::number(nResult));
}

void QuiExpressDialog::InitUi()
{
  setWindowTitle("Calc Expression");
  m_pExpressEdit = new QLineEdit(this);
  m_pResultLabel = new QLabel("0", this);
  m_pCalcBtn = new QPushButton("Calc", this);

  QHBoxLayout* pMainLayout = new QHBoxLayout(this);
  pMainLayout->addWidget(m_pExpressEdit);
  pMainLayout->addWidget(m_pResultLabel);
  pMainLayout->addWidget(m_pCalcBtn);
  pMainLayout->setMargin(5);
  pMainLayout->setSpacing(10);
}

void QuiExpressDialog::InitConnect()
{
  connect(m_pCalcBtn, &QPushButton::clicked, this, &QuiExpressDialog::SlotOnCalc);
}
