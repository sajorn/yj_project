#include "qtcalcexpress.h"
#include <QDebug>
#include <QStack>

QtCalcExpress::QtCalcExpress(QString& strExpress)
{
  m_strExpress = strExpress;
  InitOper();
}

int QtCalcExpress::CalcExpress()
{
  QString strSuffix = InffixToSuffix(m_strExpress);
  return CalcSuffix(strSuffix);
}

void QtCalcExpress::InitOper()
{
  m_mapOper.insert('+', 1);
  m_mapOper.insert('-', 1);
  m_mapOper.insert('*', 2);
  m_mapOper.insert('/', 2);
}

QString QtCalcExpress::InffixToSuffix(QString& strInfixExpress)
{
  QStack<QChar> charStack;   // 用于存储括号内容
  QString strSuffix;         // 后缀表达式
  for (int i(0); i < strInfixExpress.size(); ++i)
  {
    QChar ch = strInfixExpress.at(i);
    // 1. 如果是数字，直接加入到表达式中
    if (ch.isDigit())
    {
      strSuffix.append(ch);
    }
    // 2. 如果是运算符
    else if (m_mapOper.contains(ch))
    {
      if (charStack.size() == 0 || charStack.top() == '(')
      {
        // 如果栈顶是（，入栈
        charStack.push(ch);
      }
      else
      {
        // 将栈中的元素依次加入到表达式中，直到栈顶优先级小于该运算符，或者栈空
        while (charStack.size() > 0)
        {
          QChar topCh = charStack.top();
          if (m_mapOper[ch] > m_mapOper[topCh])
          {
            break;
          }
          charStack.pop();
          strSuffix.append(topCh);
        }
        // 将运算符加入到表达式中
        strSuffix.append(ch);
      }
    }
    // 3.如果是（，入栈
    else if (ch == '(')
    {
      charStack.push(ch);
    }
    // 4. 如果是)，将栈中的元素都输出到表达式中
    else if (ch == ')')
    {
      while (charStack.size() > 0)
      {
        QChar topCh = charStack.top();
        if (topCh == '(')
        {
          // 遇到左括号，则退出
          charStack.pop();
          break;
        }
        strSuffix.append(topCh);
        charStack.pop();
      }
    }
  }

  // 将栈中剩下元素全部加入到表达式
  while (charStack.size() > 0)
  {
    QChar topCh = charStack.top();
    if (topCh == '(')
    {
      break;
    }
    strSuffix.append(topCh);
    charStack.pop();
  }
  return strSuffix;
}

int QtCalcExpress::CalcSuffix(QString& strSuffixExpress)
{
  QStack<int> numStack;
  qDebug() << strSuffixExpress;
  foreach (QChar ch, strSuffixExpress)
  {
    // 如果是操作数，直接入栈
    if (ch.isDigit())
    {
      numStack.push(QString(ch).toInt());
    }
    else
    {
      // 取出栈顶的两个数进行运算，将运算结果压栈
      int nLeft = numStack.top();
      numStack.pop();
      int nRight = numStack.top();
      numStack.pop();
      int nResult = DoCalculate(nLeft, nRight, ch.toLatin1());
      numStack.push(nResult);
    }
  }

  return numStack.top();
}

int QtCalcExpress::DoCalculate(int nLeft, int nRight, char ch)
{
  switch (ch)
  {
  case '+':
    return nLeft + nRight;
  case '-':
    return nLeft - nRight;
  case '*':
    return nLeft * nRight;
  case '/':
    return nLeft / nRight;
  default:
    return INT32_MAX;
  }
}
