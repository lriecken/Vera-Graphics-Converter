#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->project = new Project(this);

  this->paletteWidget = new PaletteWidget(project->getPalette(), this);

  this->imageWidget = new ImageWidget(project, this);
  this->ui->paletteScrollArea->setWidget(paletteWidget);
  this->ui->centralwidget->layout()->addWidget(imageWidget);
  connect(paletteWidget, SIGNAL(PaletteChanged()), this,
          SLOT(paletteChanged()));
  connect(project->getTiles(), SIGNAL(needUiUpdate()), this, SLOT(updateUI()));
  connect(imageWidget, SIGNAL(needUiUpdate()), this, SLOT(updateUI()));
  connect(imageWidget, SIGNAL(IndicesSelected(int)), paletteWidget,
          SLOT(ShouldHighlight(int)));
  connect(imageWidget, SIGNAL(IndicesSelected(int)), this,
          SLOT(indicesSelected(int)));
  connect(project->getTiles(), SIGNAL(TilemapProgress(float)), this,
          SLOT(ReceiveProgress(float)));
  connect(project->getTiles(), SIGNAL(TilemapReady()), this,
          SLOT(TilemapReady()));
  connect(paletteWidget, SIGNAL(HightlightInPicture(int)), imageWidget,
          SLOT(NewHightlightIndex(int)));
  connect(paletteWidget, SIGNAL(HightlightInPicture(int)), this,
          SLOT(indicesSelected(int)));

  this->ui->actionShowConverted->setChecked(true);

  updateUI();
  loadSettings();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete info;
  delete ui;

  delete imageWidget;
  delete paletteWidget;

  delete project;
}

void MainWindow::setStatus(const QString &text) {
  ui->statusBar->showMessage(text);
}

void MainWindow::on_actionOpenImage_triggered() {
  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open Image"),
      settings.value("paths/lastlocation", QStandardPaths::HomeLocation)
          .toString(),
      tr("Image Files (*.png *.jpg *.jpeg *.gif *.bmp)"));

  if (fileName != "") {
    if (QFile(fileName).exists()) {
      auto dir = QFileInfo(fileName).dir();
      dir.makeAbsolute();
      settings.setValue("paths/lastlocation", dir.path());
      if (!project->getTiles()->load(fileName.toStdString())) {
        QMessageBox infoBox;
        infoBox.setText(tr("The image could not be opened."));
        infoBox.exec();
      } else {
        imageWidget->repaint();
        this->project->mark();
      }
    }
  }
}

void MainWindow::on_actionShowOriginal_triggered() {
  imageWidget->setDisplayMode(ORIGINAL);

  this->ui->actionShowOriginal->setChecked(true);
  this->ui->actionShowConverted->setChecked(false);

  imageWidget->repaint();
}

void MainWindow::on_actionShowConverted_triggered() {
  imageWidget->setDisplayMode(CONVERTED);
  this->ui->actionShowOriginal->setChecked(false);
  this->ui->actionShowConverted->setChecked(true);
  imageWidget->repaint();
}

void MainWindow::on_actionClosest_RGB_triggered() {
  this->project->getTiles()->setConversionMode(CONVERT_RGB);
  project->getTiles()->makeTiles();
  imageWidget->repaint();
}

void MainWindow::on_actionClosest_HSL_triggered() {
  project->getTiles()->setConversionMode(CONVERT_HSL);
  project->getTiles()->makeTiles();
  imageWidget->repaint();
}

void MainWindow::on_actionClosest_HSV_triggered() {
  project->getTiles()->setConversionMode(CONVERT_HSV);
  project->getTiles()->makeTiles();
  imageWidget->repaint();
}

void MainWindow::ReceiveProgress(float) {
  // this->progressBar->setValue(p * 100);
}

void MainWindow::TilemapReady() {
  imageWidget->repaint();
  paletteWidget->repaint();
  size_t nBytes = project->getTiles()->getNrOfBytes();
  float kBytes = nBytes / 1024;
  messageBuffer = tr("Binary size: ") + QString::number(nBytes) +
                  tr(" bytes - ") + QString::number(kBytes) + tr(" kB");
  if (!displayingIndex) setStatus(messageBuffer);
}

void MainWindow::paletteChanged() {
  project->getTiles()->clearBuffer();
  imageWidget->repaint();
}

void MainWindow::updateUI() {
  ui->transparentSpin->setValue(project->getTiles()->getPaletteOffset());
  this->ui->zoomSlider->setValue(imageWidget->getZoomFactor() * 100);

  switch (project->getTiles()->getConversionMode()) {
    case INDEXED:
      this->ui->actionClosest_RGB->setEnabled(false);
      this->ui->actionClosest_HSL->setEnabled(false);
      this->ui->actionClosest_HSV->setEnabled(false);
      break;
    case CONVERT_RGB:
      this->ui->actionClosest_RGB->setEnabled(true);
      this->ui->actionClosest_HSL->setEnabled(true);
      this->ui->actionClosest_HSV->setEnabled(true);
      this->ui->actionClosest_RGB->setChecked(true);
      this->ui->actionClosest_HSL->setChecked(false);
      this->ui->actionClosest_HSV->setChecked(false);
      break;
    case CONVERT_HSL:
      this->ui->actionClosest_RGB->setEnabled(true);
      this->ui->actionClosest_HSL->setEnabled(true);
      this->ui->actionClosest_HSV->setEnabled(true);
      this->ui->actionClosest_RGB->setChecked(false);
      this->ui->actionClosest_HSL->setChecked(true);
      this->ui->actionClosest_HSV->setChecked(false);
      break;
    case CONVERT_HSV:
      this->ui->actionClosest_RGB->setEnabled(true);
      this->ui->actionClosest_HSL->setEnabled(true);
      this->ui->actionClosest_HSV->setEnabled(true);
      this->ui->actionClosest_RGB->setChecked(false);
      this->ui->actionClosest_HSL->setChecked(false);
      this->ui->actionClosest_HSV->setChecked(true);
      break;
  }
  if (project->getTiles()->canTile8()) {
    this->ui->iModeTiled8Check->setEnabled(true);
  } else {
    this->ui->iModeTiled8Check->setEnabled(false);
  }
  if (project->getTiles()->canTile16()) {
    this->ui->iModeTiled16Check->setEnabled(true);
  } else {
    this->ui->iModeTiled16Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(8, 8)) {
    this->ui->spriteW08Check->setEnabled(true);
  } else {
    this->ui->spriteW08Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(16, 8)) {
    this->ui->spriteW16Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteWidth() == 16)
      project->getTiles()->setSpriteWidth(8);
    this->ui->spriteW16Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(32, 8)) {
    this->ui->spriteW32Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteWidth() == 32)
      project->getTiles()->setSpriteWidth(8);
    this->ui->spriteW32Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(64, 8)) {
    this->ui->spriteW64Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteWidth() == 64)
      project->getTiles()->setSpriteWidth(8);
    this->ui->spriteW64Check->setEnabled(false);
  }

  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(8, 8)) {
    this->ui->spriteH08Check->setEnabled(true);
  } else {
    this->ui->spriteH08Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(8, 16)) {
    this->ui->spriteH16Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteHeight() == 16)
      project->getTiles()->setSpriteHeight(8);
    this->ui->spriteH16Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(8, 32)) {
    this->ui->spriteH32Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteHeight() == 32)
      project->getTiles()->setSpriteHeight(8);
    this->ui->spriteH32Check->setEnabled(false);
  }
  if (project->getTiles()->getImageMode() == SPRITE &&
      project->getTiles()->canTile(8, 64)) {
    this->ui->spriteH64Check->setEnabled(true);
  } else {
    if (project->getTiles()->getSpriteHeight() == 64)
      project->getTiles()->setSpriteHeight(8);
    this->ui->spriteH64Check->setEnabled(false);
  }
  switch (project->getTiles()->getImageMode()) {
    case IMAGEMODE::BITMAP:
      this->ui->iModeBitmapCheck->setChecked(true);
      this->ui->iModeTiled8Check->setChecked(false);
      this->ui->iModeTiled16Check->setChecked(false);
      this->ui->iModeSpriteCheck->setChecked(false);
      this->ui->spriteWidthBox->setEnabled(false);
      this->ui->spriteHeightBox->setEnabled(false);
      this->ui->pMode1Check->setEnabled(true);
      this->ui->pMode2Check->setEnabled(true);
      this->ui->NTilesSpin->setEnabled(false);
      break;
    case IMAGEMODE::TILED8:
      this->ui->iModeBitmapCheck->setChecked(false);
      this->ui->iModeTiled8Check->setChecked(true);
      this->ui->iModeTiled16Check->setChecked(false);
      this->ui->iModeSpriteCheck->setChecked(false);
      this->ui->spriteWidthBox->setEnabled(false);
      this->ui->spriteHeightBox->setEnabled(false);
      this->ui->pMode1Check->setEnabled(true);
      this->ui->pMode2Check->setEnabled(true);
      this->ui->NTilesSpin->setEnabled(true);
      if (project->getTiles()->getNColors() == 2) {
        this->ui->NTilesSpin->setRange(1, 256);
      } else {
        this->ui->NTilesSpin->setRange(1, 1024);
      }
      break;
    case IMAGEMODE::TILED16:
      this->ui->iModeBitmapCheck->setChecked(false);
      this->ui->iModeTiled8Check->setChecked(false);
      this->ui->iModeTiled16Check->setChecked(true);
      this->ui->iModeSpriteCheck->setChecked(false);
      this->ui->spriteWidthBox->setEnabled(false);
      this->ui->spriteHeightBox->setEnabled(false);
      this->ui->pMode1Check->setEnabled(true);
      this->ui->pMode2Check->setEnabled(true);
      this->ui->NTilesSpin->setEnabled(true);
      if (project->getTiles()->getNColors() == 2) {
        this->ui->NTilesSpin->setRange(1, 256);
      } else {
        this->ui->NTilesSpin->setRange(1, 1024);
      }
      break;
    case IMAGEMODE::SPRITE:
      this->ui->iModeBitmapCheck->setChecked(false);
      this->ui->iModeTiled8Check->setChecked(false);
      this->ui->iModeTiled16Check->setChecked(false);
      this->ui->iModeSpriteCheck->setChecked(true);
      this->ui->spriteWidthBox->setEnabled(true);
      this->ui->spriteHeightBox->setEnabled(true);
      this->ui->pMode1Check->setEnabled(false);
      this->ui->pMode2Check->setEnabled(false);
      this->ui->NTilesSpin->setEnabled(true);

      this->ui->NTilesSpin->setRange(1, 128);

      break;
  }
  switch (project->getTiles()->getSpriteWidth()) {
    case 8:
      this->ui->spriteW08Check->setChecked(true);
      this->ui->spriteW16Check->setChecked(false);
      this->ui->spriteW32Check->setChecked(false);
      this->ui->spriteW64Check->setChecked(false);
      break;
    case 16:
      this->ui->spriteW08Check->setChecked(false);
      this->ui->spriteW16Check->setChecked(true);
      this->ui->spriteW32Check->setChecked(false);
      this->ui->spriteW64Check->setChecked(false);
      break;
    case 32:
      this->ui->spriteW08Check->setChecked(false);
      this->ui->spriteW16Check->setChecked(false);
      this->ui->spriteW32Check->setChecked(true);
      this->ui->spriteW64Check->setChecked(false);
      break;
    case 64:
      this->ui->spriteW08Check->setChecked(false);
      this->ui->spriteW16Check->setChecked(false);
      this->ui->spriteW32Check->setChecked(false);
      this->ui->spriteW64Check->setChecked(true);
      break;
  }
  switch (project->getTiles()->getSpriteHeight()) {
    case 8:
      this->ui->spriteH08Check->setChecked(true);
      this->ui->spriteH16Check->setChecked(false);
      this->ui->spriteH32Check->setChecked(false);
      this->ui->spriteH64Check->setChecked(false);
      break;
    case 16:
      this->ui->spriteH08Check->setChecked(false);
      this->ui->spriteH16Check->setChecked(true);
      this->ui->spriteH32Check->setChecked(false);
      this->ui->spriteH64Check->setChecked(false);
      break;
    case 32:
      this->ui->spriteH08Check->setChecked(false);
      this->ui->spriteH16Check->setChecked(false);
      this->ui->spriteH32Check->setChecked(true);
      this->ui->spriteH64Check->setChecked(false);
      break;
    case 64:
      this->ui->spriteH08Check->setChecked(false);
      this->ui->spriteH16Check->setChecked(false);
      this->ui->spriteH32Check->setChecked(false);
      this->ui->spriteH64Check->setChecked(true);
      break;
  }
  switch (project->getTiles()->getNColors()) {
    case 2:
      this->ui->pMode1Check->setChecked(true);
      this->ui->pMode2Check->setChecked(false);
      this->ui->pMode4Check->setChecked(false);
      this->ui->pMode8Check->setChecked(false);
      break;
    case 4:
      this->ui->pMode1Check->setChecked(false);
      this->ui->pMode2Check->setChecked(true);
      this->ui->pMode4Check->setChecked(false);
      this->ui->pMode8Check->setChecked(false);
      break;
    case 16:
      this->ui->pMode1Check->setChecked(false);
      this->ui->pMode2Check->setChecked(false);
      this->ui->pMode4Check->setChecked(true);
      this->ui->pMode8Check->setChecked(false);
      break;
    case 256:
      this->ui->pMode1Check->setChecked(false);
      this->ui->pMode2Check->setChecked(false);
      this->ui->pMode4Check->setChecked(false);
      this->ui->pMode8Check->setChecked(true);
      break;
  }
  this->ui->paletteOffset000Check->setChecked(
      project->getTiles()->getPaletteOffset() == 0);
  this->ui->paletteOffset016Check->setChecked(
      project->getTiles()->getPaletteOffset() == 16);
  this->ui->paletteOffset032Check->setChecked(
      project->getTiles()->getPaletteOffset() == 32);
  this->ui->paletteOffset048Check->setChecked(
      project->getTiles()->getPaletteOffset() == 48);
  this->ui->paletteOffset064Check->setChecked(
      project->getTiles()->getPaletteOffset() == 64);
  this->ui->paletteOffset080Check->setChecked(
      project->getTiles()->getPaletteOffset() == 80);
  this->ui->paletteOffset096Check->setChecked(
      project->getTiles()->getPaletteOffset() == 96);
  this->ui->paletteOffset112Check->setChecked(
      project->getTiles()->getPaletteOffset() == 112);
  this->ui->paletteOffset128Check->setChecked(
      project->getTiles()->getPaletteOffset() == 128);
  this->ui->paletteOffset144Check->setChecked(
      project->getTiles()->getPaletteOffset() == 144);
  this->ui->paletteOffset160Check->setChecked(
      project->getTiles()->getPaletteOffset() == 160);
  this->ui->paletteOffset176Check->setChecked(
      project->getTiles()->getPaletteOffset() == 176);
  this->ui->paletteOffset192Check->setChecked(
      project->getTiles()->getPaletteOffset() == 192);
  this->ui->paletteOffset208Check->setChecked(
      project->getTiles()->getPaletteOffset() == 208);
  this->ui->paletteOffset224Check->setChecked(
      project->getTiles()->getPaletteOffset() == 224);
  this->ui->paletteOffset240Check->setChecked(
      project->getTiles()->getPaletteOffset() == 240);
}

void MainWindow::indicesSelected(int i) {
  if (i != -1) {
    displayingIndex = true;
    setStatus(tr("Color Index Selected: ") + QString::number(i));
  } else {
    displayingIndex = false;
    setStatus(messageBuffer);
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (doClose()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::on_iModeBitmapCheck_clicked() {
  project->getTiles()->setImageMode(BITMAP);
}

void MainWindow::on_iModeTiled8Check_clicked() {
  project->getTiles()->setImageMode(TILED8);
}

void MainWindow::on_iModeTiled16Check_clicked() {
  project->getTiles()->setImageMode(TILED16);
}

void MainWindow::on_iModeSpriteCheck_clicked() {
  project->getTiles()->setImageMode(SPRITE);
}

void MainWindow::on_pMode1Check_clicked() {
  project->getTiles()->setNColors(2);
}

void MainWindow::on_pMode2Check_clicked() {
  project->getTiles()->setNColors(4);
}

void MainWindow::on_pMode4Check_clicked() {
  project->getTiles()->setNColors(16);
}

void MainWindow::on_pMode8Check_clicked() {
  project->getTiles()->setNColors(256);
}

void MainWindow::on_spriteW08Check_clicked() {
  project->getTiles()->setSpriteWidth(8);
}

void MainWindow::on_spriteW16Check_clicked() {
  project->getTiles()->setSpriteWidth(16);
}

void MainWindow::on_spriteW32Check_clicked() {
  project->getTiles()->setSpriteWidth(32);
}

void MainWindow::on_spriteW64Check_clicked() {
  project->getTiles()->setSpriteWidth(64);
}

void MainWindow::on_spriteH08Check_clicked() {
  project->getTiles()->setSpriteHeight(8);
}

void MainWindow::on_spriteH16Check_clicked() {
  project->getTiles()->setSpriteHeight(16);
}

void MainWindow::on_spriteH32Check_clicked() {
  project->getTiles()->setSpriteHeight(32);
}

void MainWindow::on_spriteH64Check_clicked() {
  project->getTiles()->setSpriteHeight(64);
}

void MainWindow::on_paletteOffset000Check_clicked() {
  project->getTiles()->setPaletteOffset(0);
}

void MainWindow::on_paletteOffset016Check_clicked() {
  project->getTiles()->setPaletteOffset(16);
}

void MainWindow::on_paletteOffset032Check_clicked() {
  project->getTiles()->setPaletteOffset(32);
}

void MainWindow::on_paletteOffset048Check_clicked() {
  project->getTiles()->setPaletteOffset(48);
}

void MainWindow::on_paletteOffset064Check_clicked() {
  project->getTiles()->setPaletteOffset(64);
}

void MainWindow::on_paletteOffset080Check_clicked() {
  project->getTiles()->setPaletteOffset(80);
}

void MainWindow::on_paletteOffset096Check_clicked() {
  project->getTiles()->setPaletteOffset(96);
}

void MainWindow::on_paletteOffset112Check_clicked() {
  project->getTiles()->setPaletteOffset(112);
}

void MainWindow::on_paletteOffset128Check_clicked() {
  project->getTiles()->setPaletteOffset(128);
}

void MainWindow::on_paletteOffset144Check_clicked() {
  project->getTiles()->setPaletteOffset(144);
}

void MainWindow::on_paletteOffset160Check_clicked() {
  project->getTiles()->setPaletteOffset(160);
}

void MainWindow::on_paletteOffset176Check_clicked() {
  project->getTiles()->setPaletteOffset(176);
}

void MainWindow::on_paletteOffset192Check_clicked() {
  project->getTiles()->setPaletteOffset(192);
}

void MainWindow::on_paletteOffset208Check_clicked() {
  project->getTiles()->setPaletteOffset(208);
}

void MainWindow::on_paletteOffset224Check_clicked() {
  project->getTiles()->setPaletteOffset(224);
}

void MainWindow::on_paletteOffset240Check_clicked() {
  project->getTiles()->setPaletteOffset(240);
}

void MainWindow::on_actionSave_as_Gimp_Palette_triggered() {
  QSettings settings;

  QString filename = QFileDialog::getSaveFileName(
      this, tr("Save Palette"), settings.value("paths/lastlocation").toString(),
      tr("Gimp Palette (*.gpl)"));
  if (filename != "") {
    auto dir = QFileInfo(filename);
    settings.setValue("paths/lastlocation", dir.path());
    project->getPalette()->saveGimpPalette(filename.toStdString());
  }
}

void MainWindow::on_actionLoad_gimp_palette_triggered() {
  QSettings settings;

  QString filename = QFileDialog::getOpenFileName(
      this, tr("Open Palette"), settings.value("paths/lastlocation").toString(),
      tr("Gimp Palette (*.gpl)"));
  if (filename != "") {
    if (QFile(filename).exists()) {
      auto dir = QFileInfo(filename);
      settings.setValue("paths/lastlocation", dir.path());
      project->getPalette()->loadGimpPalette(
          project->getTiles()->getPaletteOffset(), filename.toStdString());
      paletteWidget->repaint();
    }
  }
}

void MainWindow::on_zoomSlider_sliderMoved(int position) {
  // this->ui->zoomLabel->setText(QString::number(position) + "%");
  this->imageWidget->setZoomFactor((float)position / (float)100);
  imageWidget->repaint();
}

void MainWindow::on_actionReload_Vera_Default_triggered() {
  project->getPalette()->initializeVeraDefaultPalette();
  project->getTiles()->clearBuffer();
  imageWidget->repaint();
  paletteWidget->repaint();
}

void MainWindow::on_transparentSpin_valueChanged(int state) {
  project->getTiles()->setTransparentDefault(state);
}

std::vector<unsigned char> MainWindow::exportPRGAdress() {
  std::vector<unsigned char> data;
  unsigned char high = (project->getPRGAddress() >> 8) & 0xFF;
  unsigned char low = (project->getPRGAddress()) & 0xFF;
  data.push_back(low);
  data.push_back(high);
  return data;
}

void MainWindow::loadSettings() {
  QSettings settings;

  this->ui->configDock->setFloating(
      settings.value("layout/configdock/floating", false).toBool());
  this->ui->paletteDock->setFloating(
      settings.value("layout/palettedock/floating", false).toBool());
}

void MainWindow::saveSettings() {
  QSettings settings;

  settings.setValue("layout/configdock/floating", ui->configDock->isFloating());
  settings.setValue("layout/palettedock/floating",
                    ui->paletteDock->isFloating());
}

void MainWindow::on_actionExport_Bitmap_Tilemap_triggered() {
  if (!project->getTiles()->isInitialized()) {
    return;
  }
  QSettings settings;

  QString filename = QFileDialog::getSaveFileName(
      this, tr("Export Bitmap"),
      settings.value("paths/lastlocation").toString());
  if (filename != "") {
    auto dir = QFileInfo(filename);
    settings.setValue("paths/lastlocation", dir.path());

    auto data = project->getTiles()->exportVera();
    std::vector<unsigned char> prgadr;
    if (project->isSavePRGAddress()) {
      prgadr = exportPRGAdress();
    }
    size_t idx = 0;
    int counter = 1;
    std::ofstream ofs;
    if (data.size() > project->getSplitPosition()) {
      ofs = std::ofstream(
          QString(filename + QString::number(counter++)).toStdString(),
          std::ofstream::out);
    } else {
      ofs = std::ofstream(filename.toStdString(), std::ofstream::out);
    }

    if (ofs.is_open()) {
      if (project->isSavePRGAddress()) {
        ofs.put(prgadr[0]);
        ofs.put(prgadr[1]);
      }
      while (idx < data.size()) {
        ofs.put(data.at(idx++));
        if (idx == data.size()) {
          ofs.close();
          return;
        } else if (idx != 0 && idx % project->getSplitPosition() == 0) {
          ofs.close();
          ofs = std::ofstream(
              QString(filename + QString::number(counter++)).toStdString(),
              std::ofstream::out);
          if (project->isSavePRGAddress()) {
            ofs.put(prgadr[0]);
            ofs.put(prgadr[1]);
          }
        }
      }
    }
  }
}

void MainWindow::on_actionExport_Palette_triggered() {
  QSettings settings;

  QString filename = QFileDialog::getSaveFileName(
      this, tr("Export Palette"),
      settings.value("paths/lastlocation").toString());
  if (filename != "") {
    auto dir = QFileInfo(filename);
    settings.setValue("paths/lastlocation", dir.path());

    auto data = project->getPalette()->exportVera(0, 256);
    if (project->isSavePRGAddress()) {
      auto prgadr = exportPRGAdress();
      data.insert(data.begin(), prgadr.begin(), prgadr.end());
    }
    std::ofstream ofs{filename.toStdString(), std::ofstream::out};
    if (ofs.is_open()) {
      for (size_t i = 0; i < data.size(); ++i) {
        ofs.put(data.at(i));
      }
      ofs.close();
    }
  }
}

void MainWindow::on_actionExport_Palette_Section_as_Binary_triggered() {
  ExportPaletteSection exportPaletteSection(this);
  int res = exportPaletteSection.exec();
  if (res) {
    QSettings settings;

    QString filename = QFileDialog::getSaveFileName(
        this, tr("Export Palette"),
        settings.value("paths/lastlocation").toString());
    if (filename != "") {
      auto dir = QFileInfo(filename);
      settings.setValue("paths/lastlocation", dir.path());

      auto data = project->getPalette()->exportVera(
          exportPaletteSection.getFrom(), exportPaletteSection.getColors());
      if (project->isSavePRGAddress()) {
        auto prgadr = exportPRGAdress();
        data.insert(data.begin(), prgadr.begin(), prgadr.end());
      }
      std::ofstream ofs{filename.toStdString(), std::ofstream::out};
      if (ofs.is_open()) {
        for (size_t i = 0; i < data.size(); ++i) {
          ofs.put(data.at(i));
        }
        ofs.close();
      }
    }
  }
}

void MainWindow::on_actionInfo_triggered() {
  if (info == nullptr) {
    info = new Info(this);
  }
  info->exec();
}

void MainWindow::on_NTilesSpin_valueChanged(int state) {
  project->getTiles()->setMaxTiles(state);
}

void MainWindow::on_actionHelp_triggered() {
  if (help == nullptr) {
    help = new Help(this);
  }
  help->exec();
}

void MainWindow::on_actionHomepage_triggered() {
  QDesktopServices::openUrl(
      QUrl("https://github.com/lriecken/Vera-Graphics-Converter"));
}

bool MainWindow::doClose() {
  if (project->isClean()) {
    return true;
  } else {
    QMessageBox msgBox;
    msgBox.setText(tr("The project has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      on_actionSave_triggered();
      return true;
    } else if (ret == QMessageBox::Discard) {
      return true;
    }
  }
  return false;
}
void MainWindow::on_actionQuit_triggered() {
  if (doClose()) {
    QApplication::exit();
  }
}

void MainWindow::on_actionSave_triggered() {
  if (project->getProjectFilename() != "") {
    project->saveProject(project->getProjectFilename());
  } else {
    on_actionSave_as_triggered();
  }
}

void MainWindow::on_actionNew_triggered() {
  if (project->isClean()) {
    project->newProject();
  } else {
    QMessageBox msgBox;
    msgBox.setText(tr("The project has been modified."));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      on_actionSave_triggered();
      project->newProject();
    } else if (ret == QMessageBox::Discard) {
      project->newProject();
    }
  }
}

void MainWindow::on_actionSave_as_triggered() {
  QSettings settings;
  QString filename = QFileDialog::getSaveFileName(
      this, tr("Save project"), settings.value("paths/lastlocation").toString(),
      tr("Vera Graphics Converter Project (*.vgc)"));
  if (filename != "") {
    auto dir = QFileInfo(filename);
    settings.setValue("paths/lastlocation", dir.path());
    if (!project->saveProject(filename.toStdString())) {
      QMessageBox infoBox;
      infoBox.setText(tr("The project could not be saved."));
      infoBox.exec();
    }
  }
}

void MainWindow::on_actionOpen_triggered() {
  QSettings settings;
  QString filename = QFileDialog::getOpenFileName(
      this, tr("Open project"), settings.value("paths/lastlocation").toString(),
      tr("Vera Graphics Converter Project (*.vgc)"));
  if (filename != "") {
    auto dir = QFileInfo(filename);
    settings.setValue("paths/lastlocation", dir.path());
    if (!project->loadProject(filename.toStdString())) {
      QMessageBox infoBox;
      infoBox.setText(tr("The project could not be opened."));
      infoBox.exec();
    }
  }
}

void MainWindow::on_actionSettings_triggered() {
  Settings settings(project);
  settings.exec();
}
