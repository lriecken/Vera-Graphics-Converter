#ifndef EXPORTPALETTESECTION_H
#define EXPORTPALETTESECTION_H

#include <QDialog>

namespace Ui {
class ExportPaletteSection;
}

class ExportPaletteSection : public QDialog {
  Q_OBJECT

 public:
  explicit ExportPaletteSection(QWidget *parent = nullptr);
  ~ExportPaletteSection();
  int getFrom();
  int getColors();
 private slots:
  void on_from000_clicked();

  void on_from016_clicked();

  void on_from032_clicked();

  void on_from048_clicked();

  void on_from064_clicked();

  void on_from080_clicked();

  void on_from096_clicked();

  void on_from112_clicked();

  void on_from128_clicked();

  void on_from144_clicked();

  void on_from160_clicked();

  void on_from176_clicked();

  void on_from192_clicked();

  void on_from208_clicked();

  void on_from224_clicked();

  void on_from240_clicked();

  void on_n016_clicked();

  void on_n032_clicked();

  void on_n048_clicked();

  void on_n064_clicked();

  void on_n080_clicked();

  void on_n096_clicked();

  void on_n112_clicked();

  void on_n128_clicked();

  void on_n144_clicked();

  void on_n160_clicked();

  void on_n176_clicked();

  void on_n192_clicked();

  void on_n208_clicked();

  void on_n224_clicked();

  void on_n240_clicked();

  void on_n256_clicked();

 private:
  void setFrom(int n);
  void setColors(int n);
  int from = 0;
  int colors = 256;
  Ui::ExportPaletteSection *ui;
};

#endif  // EXPORTPALETTESECTION_H
