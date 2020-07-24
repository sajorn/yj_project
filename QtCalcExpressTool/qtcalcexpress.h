#ifndef QTCALCEXPRESS_H
#define QTCALCEXPRESS_H

#include <QString>
#include <QMap>
class QtCalcExpress
{
public:
  QtCalcExpress(QString& strExpress);

  int CalcExpress();

private:
  void InitOper();

  QString InffixToSuffix(QString& strInfixExpress);   // 中缀转后缀
  int CalcSuffix(QString& strSuffixExpress);          // 后缀求值
  int DoCalculate(int nLeft, int nRight, char ch);   // 计算

private:
  QString m_strExpress;

  QMap<QChar, int> m_mapOper;   // 运算符
};

#endif // QTCALCEXPRESS_H
