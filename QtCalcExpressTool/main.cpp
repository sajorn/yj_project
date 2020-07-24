#include "quiexpressdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QuiExpressDialog w;
  w.show();

  return a.exec();
}
