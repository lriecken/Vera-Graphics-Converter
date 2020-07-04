#include "project.h"

Project::Project(QObject *parent) : QObject(parent) {
  palette = new Palette(256);
  tiles = new Tiles(this, palette);
}

Tiles *Project::getTiles() { return tiles; }

Palette *Project::getPalette() { return palette; }

void Project::newProject() {
  this->projectFileName = "";
  this->palette->initializeVeraDefaultPalette();
  this->tiles->reset();
}

bool Project::saveProject(const std::string filename) {
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

bool Project::loadProject(const std::string filename) {
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

std::string Project::getProjectFilename() { return projectFileName; }

void Project::mark() { clean = false; }

bool Project::isClean() { return clean; }

Project::~Project() {
  delete tiles;
  delete palette;
}

QJsonObject Project::serialize() {
  QJsonObject root;
  root["save_prg_address"] = savePRGAddress;
  root["prg_address"] = (int)prgAddress;
  root["file_split_position"] = (int)splitPos;
  root["palette"] = palette->serialize();
  root["tiles"] = tiles->serialize();
  return root;
}

void Project::deserialize(QJsonObject &obj) {
  auto jPalette = obj["palette"].toObject();
  auto jTiles = obj["tiles"].toObject();
  splitPos = obj["file_split_position"].toInt();
  prgAddress = obj["prg_address"].toInt();
  savePRGAddress = obj["save_prg_address"].toBool();
  this->tiles->deserialize(jTiles);
  this->palette->deserialize(jPalette);
}

void Project::setSavePRGAddress(bool value) { this->savePRGAddress = value; }

bool Project::isSAvePRGAddress() { return savePRGAddress; }

void Project::setPRGAddress(unsigned int value) { this->prgAddress = value; }

unsigned int Project::getPRGAddress() { return this->prgAddress; }

void Project::setSplitPosition(size_t value) { this->splitPos = value; }

size_t Project::getSplitPosition() { return splitPos; }
