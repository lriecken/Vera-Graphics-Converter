#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QStandardPaths>
#include <fstream>

#include "exportpalettesection.h"
#include "help.h"
#include "imagewidget.h"
#include "info.h"
#include "palette.h"
#include "palettewidget.h"
#include "project.h"
#include "settings.h"
#include "tiles.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void setStatus(const QString &text);
  std::vector<unsigned char> exportPRGAdress();
  void loadSettings();
  void saveSettings();

 private slots:
  void on_actionOpenImage_triggered();

  void on_actionShowOriginal_triggered();

  void on_actionShowConverted_triggered();

  void on_actionClosest_RGB_triggered();

  void on_actionClosest_HSL_triggered();

  void on_iModeBitmapCheck_clicked();

  void on_iModeTiled8Check_clicked();

  void on_iModeTiled16Check_clicked();

  void on_iModeSpriteCheck_clicked();

  void on_pMode1Check_clicked();

  void on_pMode2Check_clicked();

  void on_pMode4Check_clicked();

  void on_pMode8Check_clicked();

  void on_spriteW08Check_clicked();

  void on_spriteW16Check_clicked();

  void on_spriteW32Check_clicked();

  void on_spriteW64Check_clicked();

  void on_spriteH08Check_clicked();

  void on_spriteH16Check_clicked();

  void on_spriteH32Check_clicked();

  void on_spriteH64Check_clicked();

  void on_paletteOffset000Check_clicked();

  void on_paletteOffset016Check_clicked();

  void on_paletteOffset032Check_clicked();

  void on_paletteOffset048Check_clicked();

  void on_paletteOffset064Check_clicked();

  void on_paletteOffset080Check_clicked();

  void on_paletteOffset096Check_clicked();

  void on_paletteOffset112Check_clicked();

  void on_paletteOffset128Check_clicked();

  void on_paletteOffset144Check_clicked();

  void on_paletteOffset160Check_clicked();

  void on_paletteOffset176Check_clicked();

  void on_paletteOffset192Check_clicked();

  void on_paletteOffset208Check_clicked();

  void on_paletteOffset224Check_clicked();

  void on_paletteOffset240Check_clicked();

  void on_actionSave_as_Gimp_Palette_triggered();

  void on_actionLoad_gimp_palette_triggered();

  void on_actionClosest_HSV_triggered();

  void on_zoomSlider_sliderMoved(int position);

  void on_actionReload_Vera_Default_triggered();

  void on_transparentSpin_valueChanged(int state);

  void on_actionExport_Bitmap_Tilemap_triggered();

  void on_actionExport_Palette_triggered();

  void on_actionInfo_triggered();

  void on_NTilesSpin_valueChanged(int state);

  void on_actionHelp_triggered();

  void on_actionHomepage_triggered();

  void on_actionQuit_triggered();

  void on_actionSave_triggered();

  void on_actionNew_triggered();

  void on_actionSave_as_triggered();

  void on_actionOpen_triggered();

  void on_actionExport_Palette_Section_as_Binary_triggered();

  void on_actionSettings_triggered();

 public slots:
  void ReceiveProgress(float);
  void TilemapReady();
  void paletteChanged();
  void updateUI();
  void indicesSelected(int i);

 private:
  void closeEvent(QCloseEvent *event);
  bool doClose();

  Ui::MainWindow *ui;
  Project *project;
  PaletteWidget *paletteWidget;
  ImageWidget *imageWidget;
  // QProgressBar *progressBar;
  bool displayingIndex = false;

  QString messageBuffer = "";

  Info *info = nullptr;
  Help *help = nullptr;
 signals:
  void imageControlChanged();
};
#endif  // MAINWINDOW_H
