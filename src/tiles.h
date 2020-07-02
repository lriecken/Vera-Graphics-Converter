#ifndef TILES_H
#define TILES_H

#include <QColor>
#include <QImage>
#include <QJsonObject>
#include <QPainter>
#include <QRgb>
#include <QThread>
#include <vector>

#include "palette.h"
#include "tile.h"

class Tiles : public QObject {
  Q_OBJECT
 public:
  Tiles(QObject *parent, Palette *palette);
  bool load(const std::string &filename);
  void reset();
  void makeTiles();

  bool canTile8();
  bool canTile16();
  bool canTile(int x, int y);
  QImage &getOriginal();
  void setConversionMode(CONVERT_VIA mode);
  void render(QPainter &qp, size_t width, size_t height, float zoomFactor,
              int offsetX, int offsetY);

  void clearBuffer();
  void setImageMode(IMAGEMODE mode);
  void setSpriteWidth(int w);
  int getSpriteWidth();
  int getSpriteHeight();
  void setSpriteHeight(int h);
  bool isInitialized();
  int getNColors();
  void setNColors(int n);
  void setPaletteOffset(int n);
  int getPaletteOffset();
  IMAGEMODE getImageMode();
  CONVERT_VIA getConversionMode();
  int getIndexAt(int x, int y, size_t width, float zoomFactor, int offsetX,
                 int offsetY);
  int getTileAt(int x, int y, size_t width, float zoomFactor, int offsetX,
                int offsetY);
  void setTransparentDefault(int i);
  std::vector<unsigned char> exportVera();
  size_t getNrOfBytes();
  int getMaxTiles();
  void setMaxTiles(int n);
  void setHightlightIndex(int idx);
  void selectTile(size_t idx) {
    this->selectedTile = idx;
    emit TilemapReady();
    emit needUiUpdate();
  }
  size_t getSelectedTileIndex() { return selectedTile; }
  Tile *getSelectedTile() {
    if (selectedTile < tiles.size()) {
      return tiles[selectedTile];
    }
    return nullptr;
  }
  QRect getTilePosition(size_t i, size_t width, size_t height, float zoomFactor,
                        int offsetX, int offsetY);

  QJsonObject serialize() {
    QJsonObject root;
    root["filename"] = QString::fromStdString(filename);
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
  void deserialize(QJsonObject &obj) {
    paletteOffset = obj["palette_offset"].toInt();
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

 private:
  Tile *makeTile(size_t width, size_t height, size_t paletteOffset,
                 size_t nColors, int offX, int offY);

  void clearTiles();
  std::string filename;
  Palette *palette;
  size_t paletteOffset = 0;
  std::vector<Tile *> tiles;
  IMAGEMODE imgMode = BITMAP;
  CONVERT_VIA convMode = CONVERT_RGB;
  size_t spriteWidth = 64;
  size_t spriteHeight = 64;
  size_t nTilesX;
  size_t nTilesY;
  size_t nColors = 256;
  SRC_MODE srcMode;
  QImage original;
  size_t originalWidth;
  size_t originalHeight;
  size_t tranparentDefault = 0;
  int tilePadding = 1;
  int maxTiles = 256;
  bool initialized = false;
  bool imageLoaded = false;
  int hightlightIndex = -1;
  size_t selectedTile = 0;
 public slots:
  void TileProgress(float);
  void TileInitialized();
 signals:
  void TilemapReady();
  void TilemapProgress(float p);
  void needUiUpdate();
};

#endif  // TILES_H
