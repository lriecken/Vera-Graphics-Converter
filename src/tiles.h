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
  void selectTile(size_t idx);
  size_t getSelectedTileIndex() { return selectedTile; }
  Tile *getSelectedTile();
  QRect getTilePosition(size_t i, size_t width, size_t height, float zoomFactor,
                        int offsetX, int offsetY);

  QJsonObject serialize();
  void deserialize(QJsonObject &obj);

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
