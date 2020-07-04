#include "tile.h"

Tile::Tile(QObject *parent, size_t width, size_t height, size_t paletteOffset,
           size_t nColors, int offX, int offY, QImage &image, CONVERT_VIA mode,
           Palette *palette, int transparentDefault)
    : QObject(parent),
      width{width},
      height{height},
      paletteOffset{paletteOffset},
      nColors{nColors},
      palette(palette) {
  worker =
      new ConversionWorker(parent, palette, &image, width, height, offX, offY,
                           paletteOffset, nColors, mode, transparentDefault);
  connect(worker, SIGNAL(progress(float)), this, SLOT(newProgres(float)));
  connect(worker, SIGNAL(finished()), this, SLOT(done()));
}

void Tile::init() { worker->start(); }

int Tile::getWidth() { return width; }

int Tile::getHeight() { return height; }

std::vector<size_t> Tile::getIndices() { return indices; }

size_t Tile::getPixelIdx(int x, int y) {
  return indices.at((y * width + x) % indices.size());
}

void Tile::setPixelIdx(int x, int y, size_t i) {
  indices.at((y * width + x) % indices.size()) = i;
}

bool Tile::setIndices(std::vector<size_t> indices) {
  if (indices.size() != this->indices.size()) {
    return false;
  }
  this->indices = indices;
  return true;
}

bool Tile::isReady() { return ready; }

void Tile::clear() { ready = false; }

QImage &Tile::getImage(int hightlightIndex) {
  if (!ready) {
    makeImage(hightlightIndex);
  }
  return buffer;
}

Tile::~Tile() {
  if (worker != nullptr) {
    disconnect(worker, SIGNAL(progress(float)), this, SLOT(newProgres(float)));
    disconnect(worker, SIGNAL(finished()), this, SLOT(done()));
    delete worker;
    worker = nullptr;
  }
}

size_t Tile::getSize() {
  if (nColors == 256) {
    return width * height;
  } else if (nColors == 16) {
    return (width * height) / 2;
  } else if (nColors == 4) {
    return (width * height) / 4;
  } else if (nColors == 2) {
    return (width * height) / 8;
  }
  return 0;
}

void Tile::setPaletteOffset(size_t n) {
  this->paletteOffset = n;
  this->worker->paletteOffset = n;
  init();
}

std::vector<unsigned char> Tile::exportVera() {
  std::vector<unsigned char> data;
  size_t i = 0;
  size_t idx;
  while (i < indices.size()) {
    if (nColors == 256) {
      idx = indices.at(i);
      idx -= paletteOffset;
      unsigned char d = idx & 0xFF;
      data.push_back(d);
    } else if (nColors == 16) {
      idx = indices.at(i);
      idx -= paletteOffset;
      unsigned char d = (idx << 4) & 0xF0;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= idx & 0x0F;
      data.push_back(d);
    } else if (nColors == 4) {
      idx = indices.at(i);
      idx -= paletteOffset;
      unsigned char d = (idx << 6) & 0b11000000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 4) & 0b00110000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 2) & 0b00001100;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx)&0b00000011;
      data.push_back(d);
    } else if (nColors == 2) {
      idx = indices.at(i);
      idx -= paletteOffset;
      unsigned char d = (idx << 7) & 0b10000000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 6) & 0b01000000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 5) & 0b00100000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 4) & 0b00010000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 3) & 0b00001000;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 2) & 0b00000100;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx << 1) & 0b00000010;
      i++;
      idx = indices.at(i);
      idx -= paletteOffset;
      d |= (idx)&0b00000001;
      data.push_back(d);
    }
    i++;
  }

  return data;
}

QJsonObject Tile::serialize() {
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

void Tile::deserialize(QJsonObject &obj) {
  this->width = obj["width"].toInt();
  this->height = obj["height"].toInt();
  this->setPaletteOffset(obj["palette_offset"].toInt());
  this->nColors = obj["nr_of_colors"].toInt();
}

void Tile::newProgres(float p) {
  currentProgress = p;
  emit progress(p);
}

void Tile::done() {
  this->indices = worker->indices;
  initComplete = true;
  clear();
  emit initialized();
}

void Tile::makeImage(int highlightIndex) {
  buffer = QImage(width, height, QImage::Format_ARGB32);
  size_t i = 0;
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      auto idx = (int)indices.at(i++);
      auto col = palette->getColor(idx);
      if (idx == highlightIndex) {
        col.comp.r -= 128;
        col.comp.g -= 128;
        col.comp.b -= 128;
      }
      buffer.setPixel(x, y, (QRgb)col.data);
    }
  }
  ready = true;
}

ConversionWorker::ConversionWorker(QObject *parent, Palette *palette,
                                   QImage *img, size_t width, size_t height,
                                   size_t offX, size_t offY,
                                   size_t paletteOffset, size_t nColors,
                                   CONVERT_VIA convMode, int transparentDefault)
    : QThread(parent),
      palette{palette},
      paletteOffset{paletteOffset},
      nColors{nColors},
      width{width},
      height{height},
      offX{offX},
      offY{offY},
      convMode{convMode},
      transparentDefault{transparentDefault},
      img{img} {}

void ConversionWorker::run() {
  indices.clear();

  if (img->depth() <= 8) {
    for (size_t y = 0; y < height; ++y) {
      for (size_t x = 0; x < width; ++x) {
        auto pidx = (unsigned char)img->pixelIndex(x + offX, y + offY);
        if (nColors == 2) {
          pidx -= paletteOffset;  // This is supposed to turn around;
          pidx &= 0b00000001;
          pidx += paletteOffset;
        }
        if (nColors == 4) {
          pidx -= paletteOffset;  // This is supposed to turn around;
          pidx &= 0b00000011;
          pidx += paletteOffset;
        }
        if (nColors == 16) {
          pidx -= paletteOffset;  // This is supposed to turn around;
          pidx &= 0b00001111;
          pidx += paletteOffset;
        }
        indices.push_back((pidx) % palette->getPaletteSize());
      }
      // if (y % 10 == 0) emit progress((float)y / (float)height);
    }
  } else {
    for (size_t y = offY; y < offY + height; ++y) {
      Color *sline = (Color *)img->scanLine(y);
      for (size_t x = offX; x < offX + width; ++x) {
        if (sline[x].comp.a == 0) {
          indices.push_back(transparentDefault);
        } else {
          if (convMode == CONVERT_RGB) {
            indices.push_back(
                palette->getClosestRGB(nColors, paletteOffset, sline[x]));
          } else if (convMode == CONVERT_HSL) {
            indices.push_back(
                palette->getClosestHSL(nColors, paletteOffset, sline[x]));
          } else if (convMode == CONVERT_HSV) {
            indices.push_back(
                palette->getClosestHSV(nColors, paletteOffset, sline[x]));
          }
        }
      }
      // if (y % 10 == 0) emit progress(((float)y-offY) / ((float)height-offY));
    }
  }
}
