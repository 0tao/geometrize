#pragma once

#include <QGraphicsView>
#include <QObject>

class QWheelEvent;

namespace geometrize
{

namespace dialog
{

/**
 * @brief The ImageJobGraphicsView class models a graphics view for viewing the images and shapes used and/or produced by image jobs.
 */
class ImageJobGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageJobGraphicsView(QWidget* parent = nullptr);

private:
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};

}

}
