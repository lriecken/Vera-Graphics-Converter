#ifndef TILE_H
#define TILE_H

#include <QColor>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QRgb>
#include <QThread>
#include <vector>

#include "palette.h"
enum CONVERT_VIA { INDEXED, CONVERT_RGB, CONVERT_HSL, CONVERT_HSV };
enum IMAGEMODE { BITMAP, TILED8, TILED16, SPRITE };

class ConversionWorker : public QThread {
  Q_OBJECT
 public:
  ConversionWorker(QObject *parent, Palette *palette, QImage *img, size_t width,
                   size_t height, size_t offX, size_t offY,
                   size_t paletteOffset, size_t nColors, CONVERT_VIA convMode,
                   int transparentDefault);
  void run() override;
  std::vector<size_t> indices;
  Palette *palette;
  size_t paletteOffset = 0;
  size_t nColors = 256;
  size_t width, height, offX, offY;
  CONVERT_VIA convMode;
  int transparentDefault;

 private:
  QImage *img;

 signals:
  void progress(float p);
};

class Tile : public QObject {
  Q_OBJECT
 public:
  Tile(QObject *parent, size_t width, size_t height, size_t paletteOffset,
       size_t nColors, int offX, int offY, QImage &image, CONVERT_VIA mode,
       Palette *palette, int transparentDefault);

  void init();
  int getWidth();
  int getHeight();
  std::vector<size_t> getIndices();
  size_t getPixelIdx(int x, int y);
  void setPixelIdx(int x, int y, size_t i);
  bool setIndices(std::vector<size_t> indices);
  bool isReady();
  void clear();
  size_t getSize();
  void setPaletteOffset(size_t n) {
    this->paletteOffset = n;
    this->worker->paletteOffset = n;
    init();
  }
  size_t getPaletteOffset() { return paletteOffset; }
  QImage &getImage(int hightlightIndex);

  ~Tile();

  std::vector<unsigned char> exportVera();

  bool initComplete = false;
  float currentProgress = 0;
  QJsonObject serialize() {
    QJsonObject root;
    root["width"] = (int)width;
    root["height"] = (int)height;
    root["palette_offset"] = (int)paletteOffset;
    root["nr_of_colors"] = (int)nColors;
    /*
    QJsonArray jIndices;
    for (size_t i = 0; i < indices.size(); ++i) {
      jIndices.append((int)indices.at(i));
    }
    root["indices"] = jIndices;
    */
    return root;
  }
  void deserialize(QJsonObject &obj) {
    this->width = obj["width"].toInt();
    this->height = obj["height"].toInt();
    this->setPaletteOffset(obj["palette_offset"].toInt());
    this->nColors = obj["nr_of_colors"].toInt();
  }
 private slots:
  void newProgres(float p);
  void done();
 signals:
  void progress(float p);
  void initialized();

 private:
  void makeImage(int highlightIndex);
  size_t width;
  size_t height;
  size_t paletteOffset;
  size_t nColors;
  std::vector<size_t> indices;
  bool ready = false;

  QImage buffer;
  Palette *palette;
  ConversionWorker *worker;
};

enum SRC_MODE { INDEXED_1, INDEXED_2, INDEXED_4, INDEXED_8, RGBA };

#endif  // TILE_H
