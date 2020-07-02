#include "exportpalettesection.h"

#include "ui_exportpalettesection.h"

ExportPaletteSection::ExportPaletteSection(QWidget *parent)
    : QDialog(parent), ui(new Ui::ExportPaletteSection) {
  ui->setupUi(this);
}

ExportPaletteSection::~ExportPaletteSection() { delete ui; }

int ExportPaletteSection::getFrom() { return from; }

int ExportPaletteSection::getColors() { return colors; }

void ExportPaletteSection::on_from000_clicked() { setFrom(0); }

void ExportPaletteSection::on_from016_clicked() { setFrom(16); }

void ExportPaletteSection::on_from032_clicked() { setFrom(32); }

void ExportPaletteSection::on_from048_clicked() { setFrom(48); }

void ExportPaletteSection::on_from064_clicked() { setFrom(64); }

void ExportPaletteSection::on_from080_clicked() { setFrom(80); }

void ExportPaletteSection::on_from096_clicked() { setFrom(96); }

void ExportPaletteSection::on_from112_clicked() { setFrom(112); }

void ExportPaletteSection::on_from128_clicked() { setFrom(128); }

void ExportPaletteSection::on_from144_clicked() { setFrom(144); }

void ExportPaletteSection::on_from160_clicked() { setFrom(160); }

void ExportPaletteSection::on_from176_clicked() { setFrom(176); }

void ExportPaletteSection::on_from192_clicked() { setFrom(192); }

void ExportPaletteSection::on_from208_clicked() { setFrom(208); }

void ExportPaletteSection::on_from224_clicked() { setFrom(224); }

void ExportPaletteSection::on_from240_clicked() { setFrom(240); }

void ExportPaletteSection::on_n016_clicked() { setColors(16); }

void ExportPaletteSection::on_n032_clicked() { setColors(32); }

void ExportPaletteSection::on_n048_clicked() { setColors(48); }

void ExportPaletteSection::on_n064_clicked() { setColors(64); }

void ExportPaletteSection::on_n080_clicked() { setColors(80); }

void ExportPaletteSection::on_n096_clicked() { setColors(96); }

void ExportPaletteSection::on_n112_clicked() { setColors(112); }

void ExportPaletteSection::on_n128_clicked() { setColors(128); }

void ExportPaletteSection::on_n144_clicked() { setColors(144); }

void ExportPaletteSection::on_n160_clicked() { setColors(160); }

void ExportPaletteSection::on_n176_clicked() { setColors(176); }

void ExportPaletteSection::on_n192_clicked() { setColors(192); }

void ExportPaletteSection::on_n208_clicked() { setColors(208); }

void ExportPaletteSection::on_n224_clicked() { setColors(224); }

void ExportPaletteSection::on_n240_clicked() { setColors(240); }

void ExportPaletteSection::on_n256_clicked() { setColors(256); }

void ExportPaletteSection::setFrom(int n) {
  from = n;
  ui->n256->setEnabled(true);
  ui->n240->setEnabled(true);
  ui->n224->setEnabled(true);
  ui->n208->setEnabled(true);
  ui->n192->setEnabled(true);
  ui->n176->setEnabled(true);
  ui->n160->setEnabled(true);
  ui->n144->setEnabled(true);
  ui->n128->setEnabled(true);
  ui->n112->setEnabled(true);
  ui->n096->setEnabled(true);
  ui->n080->setEnabled(true);
  ui->n064->setEnabled(true);
  ui->n048->setEnabled(true);
  ui->n032->setEnabled(true);
  ui->n016->setEnabled(true);

  if (from >= 16) {
    ui->n256->setEnabled(false);
    if (ui->n256->isChecked()) ui->n240->setChecked(true);
  }
  if (from >= 32) {
    ui->n240->setEnabled(false);
    if (ui->n240->isChecked()) ui->n224->setChecked(true);
  }
  if (from >= 48) {
    ui->n224->setEnabled(false);
    if (ui->n224->isChecked()) ui->n208->setChecked(true);
  }
  if (from >= 64) {
    ui->n208->setEnabled(false);
    if (ui->n208->isChecked()) ui->n192->setChecked(true);
  }
  if (from >= 80) {
    ui->n192->setEnabled(false);
    if (ui->n192->isChecked()) ui->n176->setChecked(true);
  }
  if (from >= 96) {
    ui->n176->setEnabled(false);
    if (ui->n176->isChecked()) ui->n160->setChecked(true);
  }
  if (from >= 112) {
    ui->n160->setEnabled(false);
    if (ui->n160->isChecked()) ui->n144->setChecked(true);
  }
  if (from >= 128) {
    ui->n144->setEnabled(false);
    if (ui->n144->isChecked()) ui->n128->setChecked(true);
  }
  if (from >= 144) {
    ui->n128->setEnabled(false);
    if (ui->n128->isChecked()) ui->n112->setChecked(true);
  }
  if (from >= 160) {
    ui->n112->setEnabled(false);
    if (ui->n112->isChecked()) ui->n096->setChecked(true);
  }
  if (from >= 176) {
    ui->n096->setEnabled(false);
    if (ui->n096->isChecked()) ui->n080->setChecked(true);
  }
  if (from >= 192) {
    ui->n080->setEnabled(false);
    if (ui->n080->isChecked()) ui->n064->setChecked(true);
  }
  if (from >= 208) {
    ui->n064->setEnabled(false);
    if (ui->n064->isChecked()) ui->n048->setChecked(true);
  }
  if (from >= 224) {
    ui->n048->setEnabled(false);
    if (ui->n048->isChecked()) ui->n032->setChecked(true);
  }
  if (from >= 240) {
    ui->n032->setEnabled(false);
    if (ui->n032->isChecked()) ui->n016->setChecked(true);
  }
}

void ExportPaletteSection::setColors(int n) { colors = n; }
