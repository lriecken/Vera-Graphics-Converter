#include <QApplication>
#include <QSettings>

#include "mainwindow.h"
int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("lennart-riecken");
  QCoreApplication::setApplicationName("VeraGraphicsConverter");
  QCoreApplication::setOrganizationDomain("lennart-riecken.de");
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
