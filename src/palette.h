#ifndef PALETTE_H
#define PALETTE_H

#include <math.h>

#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <QRgb>
#include <fstream>
#include <iostream>
#include <vector>

union Color {
  unsigned int data = 0;
  struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
  } comp;
};

struct ColorComp {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned int hslH;
  unsigned int hslS;
  unsigned int lightness;
  unsigned int hsvH;
  unsigned int hsvS;
  unsigned int value;
  float hslHF;
  float hslSF;
  float lightnessF;
  float hsvHF;
  float hsvSF;
  float valueF;
};

class Palette {
 public:
  Palette(size_t n);
  Color colorFrom12Bit(unsigned int c);

  size_t getClosestRGB(size_t nPixel, int paletteOffset, Color &color);
  size_t getClosestHSL(size_t nPixel, int paletteOffset, Color &color);
  size_t getClosestHSV(size_t nPixel, int paletteOffset, Color &color);
  void saveGimpPalette(const std::string &filename);

  void loadGimpPalette(int offset, const std::string &filename);
  void setColor(size_t i, QColor &color);
  void setColor(size_t i, int r, int g, int b);
  void setColor(size_t i, Color c);
  Color getColor(size_t i);
  ColorComp getColorComp(size_t i);
  QColor getQColor(size_t i);
  size_t getPaletteSize();

  void initializeVeraDefaultPalette();

  std::vector<unsigned char> exportVera(size_t from, size_t n);
  QJsonObject serialize();
  void deserialize(QJsonObject &obj);

 private:
  double getRGBDiff(ColorComp &a, ColorComp &b);
  double getHSLDiff(ColorComp &a, ColorComp &b);
  double getHSVDiff(ColorComp &a, ColorComp &b);
  std::vector<Color> entriesColor;
  std::vector<ColorComp> entriesComp;
};

#endif  // PALETTE_H
