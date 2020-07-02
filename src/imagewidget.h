#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <math.h>

#include <QColor>
#include <QDragMoveEvent>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QRgb>
#include <QThread>
#include <QWheelEvent>
#include <QWidget>
#include <iostream>

#include "palette.h"
#include "project.h"
#include "tiles.h"

enum DISPLAY { ORIGINAL, CONVERTED };

class ImageWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ImageWidget(Project *project, QWidget *parent = nullptr);

  void calculateDimensions(int &w, int &h, int &offX, int &offY);

  void paintEvent(QPaintEvent *) override;

  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *) override;
  void setDisplayMode(DISPLAY mode);
  DISPLAY getDisplayMode();

  void setZoomFactor(float v);
  float getZoomFactor();
  void resetOffsets();

 private:
  Project *project;
  DISPLAY displayMode = CONVERTED;

  float zoomFactor = 1;
  bool mouseDown = false;
  int mouseDownAtX = 0;
  int mouseDownAtY = 0;

  int drawOffsetX = 0;
  int drawOffsetY = 0;
 signals:
  void IndicesSelected(int idx);
  void needUiUpdate();
 public slots:
  void NewHightlightIndex(int idx);
};

#endif  // IMAGEWIDGET_H
