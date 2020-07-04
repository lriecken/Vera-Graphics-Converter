#include "palettewidget.h"

PaletteWidget::PaletteWidget(Palette *palette, QWidget *parent)
    : QWidget(parent) {
  this->palette = palette;
}

void PaletteWidget::paintEvent(QPaintEvent *) {
  size();
  std::cout << width() << " " << height() << std::endl;
  int newHeight = this->drawPallete();
  setMinimumHeight(newHeight);
}

void PaletteWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  int x = event->x();
  int y = event->y();
  for (size_t i = 0; i < palette->getPaletteSize(); ++i) {
    auto cell = getCellRect(i);
    if (cell.contains(x, y)) {
      Color c = palette->getColor(i);
      QColor original = QColor::fromRgb(c.comp.r, c.comp.g, c.comp.b);
      QColor selected =
          QColorDialog::getColor(original, this, tr("Select Color"));
      if (selected.isValid()) {
        palette->setColor(i, selected);
        repaint();
        emit PaletteChanged();
      }
    }
  }
}

void PaletteWidget::mousePressEvent(QMouseEvent *event) {
  int x = event->x();
  int y = event->y();
  for (size_t i = 0; i < palette->getPaletteSize(); ++i) {
    auto cell = getCellRect(i);
    if (cell.contains(x, y)) {
      emit HightlightInPicture(i);
    }
  }
}

void PaletteWidget::mouseMoveEvent(QMouseEvent *event) {
  mousePressEvent(event);
}

void PaletteWidget::mouseReleaseEvent(QMouseEvent *) {
  emit HightlightInPicture(-1);
}

int PaletteWidget::drawPallete() {
  int calculatedHeight = 0;
  try {
    QPen blackPen{QColor::fromRgb(0, 0, 0)};

    QPen hightlighPen{QColor::fromRgb(255, 255, 255)};
    hightlighPen.setWidth(3);

    if (width() > colorFrameSize) {
      QPainter *qp = new QPainter();
      qp->begin(this);
      qp->setPen(blackPen);
      for (size_t i = 0; i < palette->getPaletteSize(); ++i) {
        auto cell = getCellRect(i);
        qp->fillRect(cell, static_cast<QRgb>(palette->getColor(i).data));
        qp->drawRect(cell);
        calculatedHeight = cell.y() + cell.height();
      }
      if (highlighted >= 0) {
        auto cell = getCellRect(highlighted);
        qp->setPen(hightlighPen);
        qp->drawRect(cell);

        qp->setPen(blackPen);
      }
      qp->end();
      delete qp;
      return calculatedHeight;
    }
  } catch (const std::out_of_range &e) {
    std::cout << e.what() << std::endl;
  }
}

QRect PaletteWidget::getCellRect(int i) {
  int nCellsH = width() / colorFrameSize;
  // int nCellsV = nEntries / nCellsH;
  int cellPosX = (i % nCellsH) * colorFrameSize;
  int cellPosY = (i / nCellsH) * colorFrameSize;

  return QRect{cellPosX, cellPosY, colorFrameSize, colorFrameSize};
}

void PaletteWidget::ShouldHighlight(int idx) {
  this->highlighted = idx;
  repaint();
}
