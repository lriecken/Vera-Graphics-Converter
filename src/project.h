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
  Project(QObject *parent) : QObject(parent) {
    palette = new Palette(256);
    tiles = new Tiles(this, palette);
  }
  Tiles *getTiles() { return tiles; }
  Palette *getPalette() { return palette; }

  void newProject() {
    this->projectFileName = "";
    this->palette->initializeVeraDefaultPalette();
    this->tiles->reset();
  }
  bool saveProject(const std::string filename) {
    this->projectFileName = filename;
    auto json = serialize();
    QJsonDocument doc(json);
    QFile ofs(QString::fromStdString(filename));
    if (!ofs.open(QIODevice::WriteOnly)) {
      return false;
    }

    ofs.write(doc.toJson());

    ofs.close();
    clean = true;
    return true;
  }
  bool loadProject(const std::string filename) {
    QFile ifs(QString::fromStdString(filename));
    if (!ifs.open(QIODevice::ReadOnly)) {
      return false;
    }
    auto data = ifs.readAll();
    ifs.close();
    auto doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
      return false;
    }
    auto obj = doc.object();
    this->deserialize(obj);
    mark();
    return true;
  }
  std::string getProjectFilename() { return projectFileName; }
  void mark() { clean = false; }
  bool isClean() { return clean; }
  ~Project() {
    delete tiles;
    delete palette;
  }

  QJsonObject serialize() {
    QJsonObject root;
    root["palette"] = palette->serialize();
    root["tiles"] = tiles->serialize();
    return root;
  }
  void deserialize(QJsonObject &obj) {
    auto jPalette = obj["palette"].toObject();
    auto jTiles = obj["tiles"].toObject();
    this->tiles->deserialize(jTiles);
    this->palette->deserialize(jPalette);
  }

 private:
  Tiles *tiles;
  Palette *palette;
  bool clean = true;
  std::string projectFileName = "";
};

#endif  // PROJECT_H
