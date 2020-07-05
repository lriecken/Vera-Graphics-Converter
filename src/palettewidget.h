#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QColor>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <iostream>
#include <vector>

#include "palette.h"

class PaletteWidget : public QWidget {
  Q_OBJECT
 public:
  explicit PaletteWidget(Palette *palette, QWidget *parent = nullptr);
  void paintEvent(QPaintEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *) override;

 private:
  int drawPallete();

  QRect getCellRect(int i);

  Palette *palette;

  int colorFrameSize = 16;

 private:
  int highlighted = -1;
 public slots:
  void ShouldHighlight(int idx);

 signals:
  void PaletteChanged();
  void HightlightInPicture(int idx);
};

#endif  // PALETTEWIDGET_H
