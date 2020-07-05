#include "tiles.h"

Tiles::Tiles(QObject *parent, Palette *palette)
    : QObject(parent), palette{palette} {}

bool Tiles::load(const std::string &filename) {
  QImage tmp;
  if (tmp.load(QString::fromStdString(filename))) {
    this->filename = filename;
    if (tmp.depth() == 1) {
      srcMode = INDEXED_1;
      this->convMode = INDEXED;
    } else if (tmp.depth() == 2) {
      srcMode = INDEXED_2;
      this->convMode = INDEXED;
    } else if (tmp.depth() == 4) {
      srcMode = INDEXED_4;
      this->convMode = INDEXED;
    } else if (tmp.depth() == 8) {
      srcMode = INDEXED_8;
      this->convMode = INDEXED;
    } else {
      srcMode = RGBA;
      if (this->convMode == INDEXED) this->convMode = CONVERT_RGB;
    }
    if (tmp.depth() <= 8) {
      auto pal = tmp.colorTable();
      for (int i = 0; i < pal.size(); ++i) {
        palette->setColor(i, qRed(pal.at(i)), qGreen(pal.at(i)),
                          qBlue(pal.at(i)));
      }
    }

    original = tmp;
    originalWidth = original.width();
    originalHeight = original.height();
    imageLoaded = true;
    makeTiles();
    emit needUiUpdate();
    return true;

  } else {
    return false;
  }
}

void Tiles::reset() {
  this->original = QImage();
  this->imageLoaded = false;
  this->nColors = 256;
  this->initialized = false;
  this->hightlightIndex = -1;
  this->selectedTile = 0;
  this->paletteOffset = 0;
  this->clearBuffer();
  this->clearTiles();
  this->imgMode = BITMAP;
  this->convMode = CONVERT_RGB;

  emit needUiUpdate();
  emit TilemapReady();
}

void Tiles::makeTiles() {
  if (!imageLoaded) {
    return;
  }
  initialized = false;
  clearTiles();
  if (imgMode == BITMAP) {
    nTilesX = 1;
    nTilesY = 1;
    Tile *t =
        makeTile(originalWidth, originalHeight, paletteOffset, nColors, 0, 0);
    tiles.push_back(t);
  } else if (imgMode == TILED8) {
    nTilesX = originalWidth / 8;
    nTilesY = originalHeight / 8;
    int i = 0;
    for (size_t y = 0; y < nTilesY; ++y) {
      for (size_t x = 0; x < nTilesX; ++x) {
        if (i < maxTiles) {
          Tile *t = makeTile(8, 8, paletteOffset, nColors, x * 8, y * 8);
          tiles.push_back(t);
        }
        ++i;
      }
    }
  } else if (imgMode == TILED16) {
    nTilesX = originalWidth / 16;
    nTilesY = originalHeight / 16;
    int i = 0;
    for (size_t y = 0; y < nTilesY; ++y) {
      for (size_t x = 0; x < nTilesX; ++x) {
        if (i < maxTiles) {
          Tile *t = makeTile(16, 16, paletteOffset, nColors, x * 16, y * 16);
          tiles.push_back(t);
        }
        ++i;
      }
    }
  } else if (imgMode == SPRITE) {
    nTilesX = originalWidth / spriteWidth;
    nTilesY = originalHeight / spriteHeight;
    int i = 0;
    for (size_t y = 0; y < nTilesY; ++y) {
      for (size_t x = 0; x < nTilesX; ++x) {
        if (i < maxTiles) {
          Tile *t = makeTile(spriteWidth, spriteHeight, paletteOffset, nColors,
                             x * spriteWidth, y * spriteHeight);
          tiles.push_back(t);
        }
        ++i;
      }
    }
  }
  for (size_t i = 0; i < tiles.size(); ++i) {
    tiles.at(i)->init();
  }
}

bool Tiles::canTile8() { return canTile(8, 8); }

bool Tiles::canTile16() { return canTile(16, 16); }

bool Tiles::canTile(int x, int y) {
  return originalWidth % x == 0 && originalHeight % y == 0;
}

QImage &Tiles::getOriginal() { return original; }

void Tiles::setConversionMode(CONVERT_VIA mode) {
  this->convMode = mode;
  emit needUiUpdate();
  if (original.depth() <= 8) makeTiles();
}

void Tiles::render(QPainter &qp, size_t width, size_t height, float zoomFactor,
                   int offsetX, int offsetY) {
  if (initialized) {
    for (size_t i = 0; i < tiles.size(); ++i) {
      auto pos =
          getTilePosition(i, width, height, zoomFactor, offsetX, offsetY);

      qp.drawImage(pos, tiles.at(i)->getImage(hightlightIndex));
    }
  }
}

void Tiles::clearBuffer() {
  for (size_t i = 0; i < tiles.size(); ++i) {
    tiles.at(i)->clear();
  }
}

void Tiles::setImageMode(IMAGEMODE mode) {
  this->imgMode = mode;
  emit needUiUpdate();
  makeTiles();
}

void Tiles::setSpriteWidth(int w) {
  spriteWidth = w;
  emit needUiUpdate();
  makeTiles();
}

int Tiles::getSpriteWidth() { return spriteWidth; }

int Tiles::getSpriteHeight() { return spriteHeight; }

void Tiles::setSpriteHeight(int h) {
  spriteHeight = h;
  emit needUiUpdate();
  makeTiles();
}

bool Tiles::isInitialized() { return initialized; }

int Tiles::getNColors() { return nColors; }

void Tiles::setNColors(int n) {
  nColors = n;
  emit needUiUpdate();
  makeTiles();
}

void Tiles::setPaletteOffset(int n) {
  if (getSelectedTile() != nullptr) {
    this->getSelectedTile()->setPaletteOffset(n);
  }

  this->paletteOffset = n;
  emit needUiUpdate();
}

int Tiles::getPaletteOffset() {
  Tile *tmp = getSelectedTile();
  if (tmp != nullptr) {
    return tmp->getPaletteOffset();
  }
  return paletteOffset;
}

IMAGEMODE Tiles::getImageMode() { return imgMode; }

CONVERT_VIA Tiles::getConversionMode() { return convMode; }

int Tiles::getIndexAt(int x, int y, size_t width, float zoomFactor, int offsetX,
                      int offsetY) {
  for (size_t i = 0; i < tiles.size(); ++i) {
    auto cell = getTilePosition(i, width, 0, zoomFactor, offsetX, offsetY);
    if (cell.contains(x, y)) {
      float fx = x - cell.x();
      float fy = y - cell.y();
      fx /= zoomFactor;
      fy /= zoomFactor;
      return tiles.at(i)->getPixelIdx(fx, fy);
    }
  }
  return -1;
}

int Tiles::getTileAt(int x, int y, size_t width, float zoomFactor, int offsetX,
                     int offsetY) {
  for (size_t i = 0; i < tiles.size(); ++i) {
    auto cell = getTilePosition(i, width, 0, zoomFactor, offsetX, offsetY);
    if (cell.contains(x, y)) {
      return i;
    }
  }
  return -1;
}

void Tiles::setTransparentDefault(int i) { tranparentDefault = i; }

std::vector<unsigned char> Tiles::exportVera() {
  std::vector<unsigned char> data;
  for (size_t i = 0; i < tiles.size(); ++i) {
    auto td = tiles.at(i)->exportVera();
    data.insert(data.end(), td.begin(), td.end());
  }
  return data;
}

size_t Tiles::getNrOfBytes() {
  int n = 0;
  for (size_t i = 0; i < tiles.size(); ++i) {
    n += tiles.at(i)->getSize();
  }
  return n;
}

int Tiles::getMaxTiles() { return maxTiles; }

void Tiles::setMaxTiles(int n) {
  maxTiles = n;
  makeTiles();
}

void Tiles::setHightlightIndex(int idx) {
  this->hightlightIndex = idx;
  this->clearBuffer();
}

void Tiles::selectTile(size_t idx) {
  this->selectedTile = idx;
  emit TilemapReady();
  emit needUiUpdate();
}

Tile *Tiles::getSelectedTile() {
  if (selectedTile < tiles.size()) {
    return tiles[selectedTile];
  }
  return nullptr;
}

Tile *Tiles::makeTile(size_t width, size_t height, size_t paletteOffset,
                      size_t nColors, int offX, int offY) {
  Tile *t = new Tile(this, width, height, paletteOffset, nColors, offX, offY,
                     original, convMode, palette, tranparentDefault);
  connect(t, SIGNAL(progress(float)), this, SLOT(TileProgress(float)));
  connect(t, SIGNAL(initialized()), this, SLOT(TileInitialized()));
  return t;
}

QRect Tiles::getTilePosition(size_t i, size_t, size_t, float zoomFactor,
                             int offsetX, int offsetY) {
  int tileW = tilePadding * 2;
  int tileH = tilePadding * 2;
  switch (imgMode) {
    case IMAGEMODE::BITMAP:
      tileW += originalWidth;
      tileH += originalHeight;
      break;
    case IMAGEMODE::TILED8:
      tileW += 8;
      tileH += 8;
      break;
    case IMAGEMODE::TILED16:
      tileW += 16;
      tileH += 16;
      break;
    case IMAGEMODE::SPRITE:
      tileW += spriteWidth;
      tileH += spriteHeight;
      break;
  }
  tileW *= zoomFactor;
  tileH *= zoomFactor;
  // int curNTilesX = width / tileW;
  int curNTilesX = nTilesX;
  if (curNTilesX == 0) curNTilesX = 1;
  int nx = sqrt(tiles.size());
  int y = i / nx;
  int x = i % nx;
  return QRect{x * tileW + tilePadding - offsetX,
               y * tileH + tilePadding - offsetY, tileW - 2 * tilePadding,
               tileH - 2 * tilePadding};
}

QJsonObject Tiles::serialize() {
  QJsonObject root;
  root["filename"] = QString::fromStdString(filename);
  root["max_tiles"] = maxTiles;
  root["palette_offset"] = (int)paletteOffset;
  root["nr_of_colors"] = (int)nColors;
  root["sprite_width"] = (int)spriteWidth;
  root["sprite_height"] = (int)spriteHeight;
  switch (imgMode) {
    case IMAGEMODE::BITMAP:
      root["imagemode"] = "bitmap";
      break;
    case IMAGEMODE::TILED8:
      root["imagemode"] = "tiled-8";
      break;
    case IMAGEMODE::TILED16:
      root["imagemode"] = "tiled-16";
      break;
    case IMAGEMODE::SPRITE:
      root["imagemode"] = "sprite";
      break;
  }
  switch (convMode) {
    case INDEXED:
      root["conversion"] = "INDEXED";
      break;
    case CONVERT_RGB:
      root["conversion"] = "RGB";
      break;
    case CONVERT_HSL:
      root["conversion"] = "HSL";
      break;
    case CONVERT_HSV:
      root["conversion"] = "HSV";
      break;
  }
  QJsonArray jTiles;
  for (size_t i = 0; i < tiles.size(); ++i) {
    jTiles.append(tiles.at(i)->serialize());
  }
  root["tiles"] = jTiles;
  return root;
}

void Tiles::deserialize(QJsonObject &obj) {
  paletteOffset = obj["palette_offset"].toInt();
  maxTiles = obj["max_tiles"].toInt();
  nColors = obj["nr_of_colors"].toInt();
  spriteWidth = obj["sprite_width"].toInt();
  spriteHeight = obj["sprite_height"].toInt();
  auto jIM = obj["imagemode"].toString().toStdString();
  if (jIM == "bitmap") imgMode = BITMAP;
  if (jIM == "tiled-8") imgMode = TILED8;
  if (jIM == "tiled-16") imgMode = TILED16;
  if (jIM == "sprite") imgMode = SPRITE;
  auto jCM = obj["conversion"].toString().toStdString();
  if (jCM == "INDEXED") convMode = INDEXED;
  if (jCM == "RGB") convMode = CONVERT_RGB;
  if (jCM == "HSL") convMode = CONVERT_HSL;
  if (jCM == "HSV") convMode = CONVERT_HSV;

  this->load(obj["filename"].toString().toStdString());

  auto jTiles = obj["tiles"].toArray();
  for (int i = 0; i < jTiles.size(); ++i) {
    if ((size_t)i < tiles.size()) {
      auto jTile = jTiles.at(i).toObject();
      tiles.at(i)->deserialize(jTile);
    }
  }
  for (size_t i = 0; i < tiles.size(); ++i) {
    tiles.at(i)->clear();
  }
}

void Tiles::clearTiles() {
  for (size_t i = 0; i < tiles.size(); ++i) {
    disconnect(tiles.at(i), SIGNAL(progress(float)), this,
               SLOT(TileProgress(float)));
    disconnect(tiles.at(i), SIGNAL(initialized()), this,
               SLOT(TileInitialized()));
  }
  selectedTile = 0;
  tiles.clear();
}

void Tiles::TileProgress(float) {
  float pCum = 0;
  for (size_t i = 0; i < tiles.size(); ++i) {
    pCum += tiles.at(i)->currentProgress;
  }
  emit TilemapProgress(pCum / (float)tiles.size());
}

void Tiles::TileInitialized() {
  bool notFinished = false;
  for (size_t i = 0; i < tiles.size(); ++i) {
    if (!tiles.at(i)->initComplete) notFinished = true;
  }
  if (!notFinished) {
    this->initialized = true;
    emit TilemapReady();
  }
}
