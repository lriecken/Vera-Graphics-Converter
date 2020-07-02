#include "palette.h"

Palette::Palette(size_t n) {
  for (size_t i = 0; i < n; ++i) {
    entriesColor.push_back(Color{});
    entriesComp.push_back(ColorComp{});
  }

  initializeVeraDefaultPalette();
}

Color Palette::colorFrom12Bit(unsigned int c) {
  Color color{};
  color.comp.r = ((c >> 8) & 0x0f) << 4;
  color.comp.g = ((c >> 4) & 0x0f) << 4;
  color.comp.b = ((c >> 0) & 0x0f) << 4;
  color.comp.a = 0xff;
  return color;
}

size_t Palette::getClosestRGB(size_t nPixel, int paletteOffset, Color &color) {
  size_t closest = 0;
  double vlength = 9999999999999;
  ColorComp tC{};
  tC.r = color.comp.r;
  tC.g = color.comp.g;
  tC.b = color.comp.b;

  for (size_t i = paletteOffset; i < nPixel + paletteOffset; ++i) {
    ColorComp pC = getColorComp(i);
    double curV = getRGBDiff(tC, pC);
    if (curV < vlength) {
      closest = i % getPaletteSize();
      vlength = curV;
    }
  }
  return closest;
}

size_t Palette::getClosestHSL(size_t nPixel, int paletteOffset, Color &color) {
  size_t closest = 0;
  double vlength = 9999999999999;
  auto qc = QColor::fromRgb(color.comp.r, color.comp.g, color.comp.b);
  ColorComp tc;
  tc.hslHF = qc.hslHueF();
  tc.hslSF = qc.hslSaturationF();
  tc.lightnessF = qc.lightnessF();
  for (size_t i = paletteOffset; i < nPixel + paletteOffset; ++i) {
    auto pc = getColorComp(i);
    double curV = getHSLDiff(tc, pc);
    if (curV < vlength) {
      closest = i % getPaletteSize();
      vlength = curV;
    }
  }
  return closest;
}

size_t Palette::getClosestHSV(size_t nPixel, int paletteOffset, Color &color) {
  size_t closest = 0;
  double vlength = 9999999999999;
  QColor qc = QColor::fromRgb(color.comp.r, color.comp.g, color.comp.b);
  ColorComp tc;
  tc.hsvHF = qc.hsvHueF();
  tc.hsvSF = qc.hsvSaturationF();
  tc.valueF = qc.valueF();
  for (size_t i = paletteOffset; i < nPixel + paletteOffset; ++i) {
    auto pc = getColorComp(i);
    double curV = getHSVDiff(tc, pc);
    if (curV < vlength) {
      closest = i % getPaletteSize();
      vlength = curV;
    }
  }
  return closest;
}

void Palette::saveGimpPalette(const std::string &filename) {
  std::ofstream ofs{filename, std::ofstream::out};
  if (ofs.is_open()) {
    ofs << "GIMP Palette" << std::endl
        << "Channels: RGBA" << std::endl
        << "#" << std::endl;
    for (size_t i = 0; i < getPaletteSize(); ++i) {
      auto c = getColor(i);
      ofs << (int)c.comp.r << "\t" << (int)c.comp.g << "\t" << (int)c.comp.b
          << "\t"
          << "255"
          << "\tUntitled" << std::endl;
    }
    ofs.close();
  }
}

void Palette::loadGimpPalette(int offset, const std::string &filename) {
  std::ifstream ifs{filename};
  std::string line;
  int idx = offset;
  if (ifs.is_open()) {
    while (getline(ifs, line)) {
      QString l = QString::fromStdString(line);
      if (l.at(0) == '#') continue;
      auto compList = l.split('\t', QString::SkipEmptyParts);
      if (compList.size() == 1) {
        compList = l.split(' ', QString::SkipEmptyParts);
      }
      Color c;

      if (compList.size() > 3) {
        c.comp.r = compList.at(0).toInt();
        c.comp.g = compList.at(1).toInt();
        c.comp.b = compList.at(2).toInt();
        c.comp.a = 255;
        this->setColor(idx, c);
        ++idx;
      }
    }
    ifs.close();
  }
}

void Palette::setColor(size_t i, QColor &color) {
  Color c;
  ColorComp cc;
  c.comp.a = 255;
  c.comp.r = color.red() & 0xF0;
  c.comp.g = color.green() & 0xF0;
  c.comp.b = color.blue() & 0xF0;
  cc.r = c.comp.r;
  cc.g = c.comp.g;
  cc.b = c.comp.b;
  cc.hslH = color.hslHue();
  cc.hslS = color.hslSaturation();
  cc.lightness = color.lightness();
  cc.hsvH = color.hsvHue();
  cc.hsvS = color.hsvSaturation();
  cc.value = color.value();
  cc.hslHF = color.hslHueF();
  cc.hslSF = color.hslSaturationF();
  cc.lightnessF = color.lightnessF();
  cc.hsvHF = color.hsvHueF();
  cc.hsvSF = color.hsvSaturationF();
  cc.valueF = color.valueF();
  entriesColor[i % entriesColor.size()] = c;
  entriesComp[i % entriesComp.size()] = cc;
}

void Palette::setColor(size_t i, int r, int g, int b) {
  auto color = QColor::fromRgb(r, g, b);
  setColor(i, color);
}

void Palette::setColor(size_t i, Color c) {
  auto color = QColor::fromRgb(c.comp.r, c.comp.g, c.comp.b);
  setColor(i, color);
}

Color Palette::getColor(size_t i) {
  return entriesColor[i % entriesColor.size()];
}

ColorComp Palette::getColorComp(size_t i) {
  return entriesComp[i % entriesColor.size()];
}

QColor Palette::getQColor(size_t i) {
  auto c = getColor(i);
  return QColor::fromRgb(c.comp.r, c.comp.g, c.comp.b);
}

size_t Palette::getPaletteSize() { return entriesColor.size(); }

void Palette::initializeVeraDefaultPalette() {
  unsigned int colors[] = {
      0x000, 0xfff, 0x800, 0xafe, 0xc4c, 0x0c5, 0x00a, 0xee7, 0xd85, 0x640,
      0xf77, 0x333, 0x777, 0xaf6, 0x08f, 0xbbb, 0x000, 0x111, 0x222, 0x333,
      0x444, 0x555, 0x666, 0x777, 0x888, 0x999, 0xaaa, 0xbbb, 0xccc, 0xddd,
      0xeee, 0xfff, 0x211, 0x433, 0x644, 0x866, 0xa88, 0xc99, 0xfbb, 0x211,
      0x422, 0x633, 0x844, 0xa55, 0xc66, 0xf77, 0x200, 0x411, 0x611, 0x822,
      0xa22, 0xc33, 0xf33, 0x200, 0x400, 0x600, 0x800, 0xa00, 0xc00, 0xf00,
      0x221, 0x443, 0x664, 0x886, 0xaa8, 0xcc9, 0xfeb, 0x211, 0x432, 0x653,
      0x874, 0xa95, 0xcb6, 0xfd7, 0x210, 0x431, 0x651, 0x862, 0xa82, 0xca3,
      0xfc3, 0x210, 0x430, 0x640, 0x860, 0xa80, 0xc90, 0xfb0, 0x121, 0x343,
      0x564, 0x786, 0x9a8, 0xbc9, 0xdfb, 0x121, 0x342, 0x463, 0x684, 0x8a5,
      0x9c6, 0xbf7, 0x120, 0x241, 0x461, 0x582, 0x6a2, 0x8c3, 0x9f3, 0x120,
      0x240, 0x360, 0x480, 0x5a0, 0x6c0, 0x7f0, 0x121, 0x343, 0x465, 0x686,
      0x8a8, 0x9ca, 0xbfc, 0x121, 0x242, 0x364, 0x485, 0x5a6, 0x6c8, 0x7f9,
      0x020, 0x141, 0x162, 0x283, 0x2a4, 0x3c5, 0x3f6, 0x020, 0x041, 0x061,
      0x082, 0x0a2, 0x0c3, 0x0f3, 0x122, 0x344, 0x466, 0x688, 0x8aa, 0x9cc,
      0xbff, 0x122, 0x244, 0x366, 0x488, 0x5aa, 0x6cc, 0x7ff, 0x022, 0x144,
      0x166, 0x288, 0x2aa, 0x3cc, 0x3ff, 0x022, 0x044, 0x066, 0x088, 0x0aa,
      0x0cc, 0x0ff, 0x112, 0x334, 0x456, 0x668, 0x88a, 0x9ac, 0xbcf, 0x112,
      0x224, 0x346, 0x458, 0x56a, 0x68c, 0x79f, 0x002, 0x114, 0x126, 0x238,
      0x24a, 0x35c, 0x36f, 0x002, 0x014, 0x016, 0x028, 0x02a, 0x03c, 0x03f,
      0x112, 0x334, 0x546, 0x768, 0x98a, 0xb9c, 0xdbf, 0x112, 0x324, 0x436,
      0x648, 0x85a, 0x96c, 0xb7f, 0x102, 0x214, 0x416, 0x528, 0x62a, 0x83c,
      0x93f, 0x102, 0x204, 0x306, 0x408, 0x50a, 0x60c, 0x70f, 0x212, 0x434,
      0x646, 0x868, 0xa8a, 0xc9c, 0xfbe, 0x211, 0x423, 0x635, 0x847, 0xa59,
      0xc6b, 0xf7d, 0x201, 0x413, 0x615, 0x826, 0xa28, 0xc3a, 0xf3c, 0x201,
      0x403, 0x604, 0x806, 0xa08, 0xc09, 0xf0b};
  for (size_t i = 0; i < getPaletteSize(); ++i) {
    setColor(i, colorFrom12Bit(colors[i]));
  }
}

std::vector<unsigned char> Palette::exportVera(size_t from, size_t n) {
  std::vector<unsigned char> data;
  for (size_t i = from; i < from + n; ++i) {
    Color c = entriesColor.at(i % entriesColor.size());
    unsigned char r = (c.comp.r) >> 4;
    unsigned char g = (c.comp.g) >> 4;
    unsigned char b = (c.comp.b) >> 4;
    unsigned char high = r & 0x0F;
    unsigned char low = (g << 4) | (b);
    data.push_back(low);  // Little endian
    data.push_back(high);
  }

  return data;
}

double Palette::getRGBDiff(ColorComp &a, ColorComp &b) {
  float rDiff = (float)a.r - (float)b.r;
  float gDiff = (float)a.g - (float)b.g;
  float bDiff = (float)a.b - (float)b.b;
  return sqrt(rDiff * rDiff + gDiff * gDiff + bDiff * bDiff);
}

double Palette::getHSLDiff(ColorComp &a, ColorComp &b) {
  float hDiff = (a.hslHF - b.hslHF);
  float sDiff = a.hslSF - b.hslSF;
  float lDiff = a.lightnessF - b.lightnessF;
  return sqrt(hDiff * hDiff + sDiff * sDiff + lDiff * lDiff);
}

double Palette::getHSVDiff(ColorComp &a, ColorComp &b) {
  float hDiff = a.hsvHF - b.hsvHF;
  float sDiff = a.hsvSF - b.hsvSF;
  float vDiff = a.valueF - b.valueF;
  return sqrt(hDiff * hDiff + sDiff * sDiff + vDiff * vDiff);
}
