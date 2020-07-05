#include "settings.h"

#include "ui_settings.h"

Settings::Settings(Project *project, QWidget *parent)
    : QDialog(parent), ui(new Ui::Settings) {
  ui->setupUi(this);
  this->project = project;
  int splitV = 2;
  while (splitV < 128) {
    ui->splitAtCombo->addItem(QString::number(splitV) + "kB");
    splitV += 2;
  }
  ui->splitAtCombo->setCurrentIndex(ui->splitAtCombo->count() - 1);

  ui->savePRGHeaderCheck->setChecked(project->isSavePRGAddress());
  ui->prgHeaderEdit->setValue(project->getPRGAddress());
  ui->splitAtCombo->setCurrentIndex((project->getSplitPosition() / 2048) - 1);
}

Settings::~Settings() { delete ui; }

void Settings::on_savePRGHeaderCheck_stateChanged(int state) {
  project->setSavePRGAddress(state);
}

void Settings::on_prgHeaderEdit_valueChanged(int state) {
  project->setPRGAddress(state);
}

void Settings::on_splitAtCombo_currentIndexChanged(int index) {
  project->setSplitPosition((index + 1) * 2048);
}
