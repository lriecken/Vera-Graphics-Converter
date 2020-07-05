#ifndef PROJECT_H
#define PROJECT_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <string>

#include "palette.h"
#include "tiles.h"

class Project : public QObject {
  Q_OBJECT
 public:
  Project(QObject *parent);
  Tiles *getTiles();
  Palette *getPalette();

  void newProject();
  bool saveProject(const std::string filename);
  bool loadProject(const std::string filename);
  std::string getProjectFilename();
  void mark();
  bool isClean();
  ~Project();

  QJsonObject serialize();
  void deserialize(QJsonObject &obj);
  void setSavePRGAddress(bool value);
  bool isSavePRGAddress();
  void setPRGAddress(unsigned int value);
  unsigned int getPRGAddress();
  void setSplitPosition(size_t value);
  size_t getSplitPosition();

 private:
  Tiles *tiles;
  Palette *palette;
  bool clean = true;
  std::string projectFileName = "";
  bool savePRGAddress = true;
  unsigned int prgAddress = 0;
  size_t splitPos = 0;
};

#endif  // PROJECT_H
