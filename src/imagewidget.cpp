#include "imagewidget.h"

ImageWidget::ImageWidget(Project *project, QWidget *parent) : QWidget(parent) {
  this->project = project;
}

void ImageWidget::calculateDimensions(int &w, int &h, int &offX, int &offY) {
  float imgRatio = (float)project->getTiles()->getOriginal().width() /
                   (float)project->getTiles()->getOriginal().height();
  float screenRation = (float)width() / (float)height();
  if (screenRation > imgRatio) {
    w = (float)height() * imgRatio;
    h = (float)height();
    offX = ((float)width() - w) / 2;
    offY = 0;
  } else {
    w = (float)width();
    h = (float)width() * (1 / imgRatio);
    offX = 0;
    offY = ((float)height() - h) / 2;
  }
}

void ImageWidget::paintEvent(QPaintEvent *) {
  QImage &original = project->getTiles()->getOriginal();
  if (original.width() > 0 && original.height() > 0) {
    if (displayMode == ORIGINAL || !project->getTiles()->isInitialized()) {
      // int w, h, offX, offY;
      // calculateDimensions(w, h, offX, offY);
      QPainter *qp = new QPainter();
      qp->begin(this);
      qp->drawImage(
          QRect(-drawOffsetX, -drawOffsetY, original.width() * zoomFactor,
                original.height() * zoomFactor),
          original);
      qp->end();
      delete qp;
    } else {
      // int w, h, offX, offY;
      // calculateDimensions(w, h, offX, offY);
      QPainter *qp = new QPainter();
      qp->begin(this);
      project->getTiles()->render(*qp, width(), height(), zoomFactor,
                                  drawOffsetX, drawOffsetY);

      QRect cell = project->getTiles()->getTilePosition(
          project->getTiles()->getSelectedTileIndex(), width(), height(),
          zoomFactor, drawOffsetX, drawOffsetY);
      qp->drawRect(cell.x() - 1, cell.y() - 1, cell.width() + 2,
                   cell.height() + 2);
      qp->end();
      delete qp;
    }
  }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event) {
  if (mouseDown) {
    int relX = mouseDownAtX - event->x();
    int relY = mouseDownAtY - event->y();
    drawOffsetX += relX;
    drawOffsetY += relY;
    mouseDownAtX = event->x();
    mouseDownAtY = event->y();
    repaint();
  }
}

void ImageWidget::wheelEvent(QWheelEvent *event) {
  auto delta = event->angleDelta().y();
  if (delta > 0) {
    zoomFactor += 0.1;
  } else {
    zoomFactor -= 0.1;
  }

  if (zoomFactor < 0.1) {
    zoomFactor = 0.1;
  }
  emit needUiUpdate();
  repaint();
}

void ImageWidget::mousePressEvent(QMouseEvent *event) {
  if (project->getTiles()->isInitialized()) {
    int palIdx = project->getTiles()->getIndexAt(
        event->x(), event->y(), width(), zoomFactor, drawOffsetX, drawOffsetY);
    int idx = project->getTiles()->getTileAt(
        event->x(), event->y(), width(), zoomFactor, drawOffsetX, drawOffsetY);
    emit IndicesSelected(palIdx);
    if (idx >= 0) {
      project->getTiles()->selectTile(idx);
    }
  }
  mouseDownAtX = event->x();
  mouseDownAtY = event->y();
  mouseDown = true;
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *) {
  mouseDown = false;
  emit IndicesSelected(-1);
}

void ImageWidget::setDisplayMode(DISPLAY mode) { this->displayMode = mode; }

DISPLAY ImageWidget::getDisplayMode() { return displayMode; }

void ImageWidget::setZoomFactor(float v) { zoomFactor = v; }

float ImageWidget::getZoomFactor() { return zoomFactor; }

void ImageWidget::resetOffsets() {
  drawOffsetX = 0;
  drawOffsetY = 0;
}

void ImageWidget::NewHightlightIndex(int idx) {
  project->getTiles()->setHightlightIndex(idx);
  repaint();
}
