#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

#include "project.h"
namespace Ui {
class Settings;
}

class Settings : public QDialog {
  Q_OBJECT

 public:
  explicit Settings(Project *project, QWidget *parent = nullptr);
  ~Settings();

 private slots:
  void on_savePRGHeaderCheck_stateChanged(int arg1);

  void on_prgHeaderEdit_valueChanged(int state);

  void on_splitAtCombo_currentIndexChanged(int index);

 private:
  Ui::Settings *ui;
  Project *project;
};

#endif  // SETTINGS_H
