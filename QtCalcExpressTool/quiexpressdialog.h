#ifndef QUIEXPRESSDIALOG_H
#define QUIEXPRESSDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;
class QuiExpressDialog : public QDialog
{
  Q_OBJECT
public:
  QuiExpressDialog();

public slots:
  void SlotOnCalc();

private:
  void InitUi();
  void InitConnect();

private:
  QLineEdit* m_pExpressEdit{nullptr};
  QLabel* m_pResultLabel{nullptr};
  QPushButton* m_pCalcBtn{nullptr};
};

#endif // QUIEXPRESSDIALOG_H
