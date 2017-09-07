#pragma once

#include <memory>

#include <QWidget>

namespace geometrize
{

namespace dialog
{

/**
 * @brief The ImageJobStatsWidget implements a widget for statistics relating to image job progress, such as the number of shapes created so far.
 */
class ImageJobStatsWidget : public QWidget
{
    Q_OBJECT

public:
    enum ImageJobStatus {
        STOPPED,
        RUNNING
    };

    ImageJobStatsWidget(QWidget* parent = nullptr);
    ~ImageJobStatsWidget();

    void setJobId(std::size_t id);
    void setTimeRunning(int millis);
    void setCurrentStatus(ImageJobStatus status);
    void setShapeCount(std::size_t shapeCount);
    void setSimilarity(float similarity);
    void setImageDimensions(std::uint32_t width, std::uint32_t height);

private:
    class ImageJobStatsWidgetImpl;
    std::unique_ptr<ImageJobStatsWidgetImpl> d;
};

}

}
