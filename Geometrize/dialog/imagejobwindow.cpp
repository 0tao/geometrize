#include "imagejobwindow.h"
#include "ui_imagejobwindow.h"

#include <assert.h>

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

#include "geometrize/bitmap/bitmap.h"
#include "geometrize/core.h"
#include "geometrize/commonutil.h"
#include "geometrize/exporter/bitmapdataexporter.h"
#include "geometrize/exporter/bitmapexporter.h"
#include "geometrize/runner/imagerunneroptions.h"
#include "geometrize/exporter/svgexporter.h"

#include "common/uiactions.h"
#include "constants.h"
#include "dialog/aboutdialog.h"
#include "dialog/collapsiblepanel.h"
#include "dialog/imagejobpixmapgraphicsitem.h"
#include "dialog/imagejobscene.h"
#include "dialog/svgpreviewdialog.h"
#include "dialog/scripteditorwidget.h"
#include "exporter/gifexporter.h"
#include "exporter/imageexporter.h"
#include "exporter/canvasanimationexporter.h"
#include "exporter/shapedataexporter.h"
#include "exporter/webglanimationexporter.h"
#include "image/imageloader.h"
#include "job/imagejob.h"
#include "strings.h"
#include "util.h"

namespace geometrize
{

namespace dialog
{

class ImageJobWindow::ImageJobWindowImpl
{
public:
    ImageJobWindowImpl(ImageJobWindow* pQ) : ui{std::make_unique<Ui::ImageJobWindow>()}, q{pQ}, m_job{nullptr}, m_running{false}, m_initialJobImage{nullptr}
    {
        ui->setupUi(q);

        ui->imageView->setScene(&m_scene);

        setupScriptEditPanels();

        connect(ui->targetOpacitySlider, &QSlider::valueChanged, [this](int value) {
            ui->targetImageOpacityValueLabel->setText(QString::number(value));

            m_scene.setTargetPixmapOpacity(value * 0.01f);
        });
    }
    ImageJobWindowImpl operator=(const ImageJobWindowImpl&) = delete;
    ImageJobWindowImpl(const ImageJobWindowImpl&) = delete;
    ~ImageJobWindowImpl() = default;

    void close()
    {
        q->close();
    }

    void setImageJob(const std::shared_ptr<job::ImageJob>& job)
    {
        m_job = job;

        m_initialJobImage = std::make_unique<geometrize::Bitmap>(m_job->getCurrent());

        setupOverlayImages();
        updateWorkingImage();

        setDisplayName(QString::fromStdString(m_job->getDisplayName()));

        syncUserInterfaceWithOptions();

        connect(job.get(), &job::ImageJob::signal_modelDidStep, [this](std::vector<geometrize::ShapeResult> shapes) {
            updateWorkingImage();

            if(m_running) {
                stepModel();
            }

            std::copy(shapes.begin(), shapes.end(), std::back_inserter(m_shapes));
            ui->shapeCountValueLabel->setText(QString::number(m_shapes.size()));
        });
    }

    void toggleRunning()
    {
        m_running = !m_running;

        if(m_running) {
            stepModel();
            ui->runStopButton->setText(tr("Stop"));
        } else {
            ui->runStopButton->setText(tr("Run"));
        }
    }

    void stepModel()
    {
        m_job->stepModel();
    }

    void clearModel()
    {
        // TODO reset or recreate the runner i.e. ensure the runner stops/the next shape produced is not added dispose of shapes
        // TODO reset the current image to whatever the target image is
        setImageJob(m_job); // TODO
    }

    void revealLaunchWindow()
    {
        if(common::ui::isLaunchWindowOpen()) {
            common::ui::bringLaunchWindowToFront();
        } else {
            common::ui::openLaunchWindow();
        }
    }

    void loadSettingsTemplate()
    {
        const QString path{common::ui::openLoadImageJobSettingsDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        m_job->getPreferences().load(path.toStdString());

        syncUserInterfaceWithOptions();
    }

    void saveSettingsTemplate() const
    {
        const QString path{common::ui::openSaveImageJobSettingsDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        m_job->getPreferences().save(path.toStdString());
    }

    void saveImage() const
    {
        const QString path{common::ui::openSaveImagePathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        geometrize::exporter::exportImage(m_job->getCurrent(), path.toStdString());
    }

    void previewSVG() const
    {
        common::ui::openSVGPreviewPage(q);
    }

    void saveSVG() const
    {
        const QString path{common::ui::openSaveSVGPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        const std::string data{geometrize::exporter::exportSVG(m_shapes, m_job->getCurrent().getWidth(), m_job->getCurrent().getHeight(), geometrize::commonutil::getAverageImageColor(m_job->getTarget()))};
        util::writeStringToFile(data, path.toStdString());
    }

    void saveRasterizedSVG() const
    {
        const QString path{common::ui::openSaveRasterizedSVGPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        const int width{ui->svgImageWidthSpinBox->value()};
        const int height{ui->svgImageHeightSpinBox->value()};

        const std::string data{geometrize::exporter::exportSVG(m_shapes, m_job->getCurrent().getWidth(), m_job->getCurrent().getHeight(), geometrize::commonutil::getAverageImageColor(m_job->getTarget()))};
        const QByteArray arrayData(data.c_str(), static_cast<int>(data.length()));
        QSvgRenderer renderer;
        renderer.load(arrayData);

        if(!renderer.isValid()) {
            assert(0 && "SVG renderer has invalid state");
            return;
        }

        QPainter painter;
        QImage image(width, height, QImage::Format_RGBA8888);
        image.fill(0);

        painter.begin(&image);
        renderer.render(&painter);
        painter.end();

        geometrize::exporter::exportImage(image, path.toStdString());
    }

    void saveGeometryData() const
    {
        const QString path{common::ui::openSaveGeometryDataPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        geometrize::exporter::ShapeDataFormat format = exporter::ShapeDataFormat::JSON;
        if(path.endsWith("json")) {
            format = geometrize::exporter::ShapeDataFormat::JSON;
        } else if(path.endsWith("txt")) {
            format = geometrize::exporter::ShapeDataFormat::CUSTOM_ARRAY;
        }

        const std::string data{geometrize::exporter::exportShapeData(m_shapes, format)};
        util::writeStringToFile(data, path.toStdString());
    }

    void saveGIF() const
    {
        const QString path{common::ui::openSaveGIFPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        geometrize::exporter::exportGIF(*m_initialJobImage, m_job->getTarget(), m_shapes, path.toStdString());
    }

    void saveCanvasAnimation() const
    {
        const QString path{common::ui::openSaveCanvasAnimationPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        const std::string pageSource{geometrize::exporter::exportCanvasAnimation(m_shapes)};
        util::writeStringToFile(pageSource, path.toStdString());
    }

    void saveWebGLAnimation() const
    {
        const QString path{common::ui::openSaveWebGLPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        const std::string pageSource{geometrize::exporter::exportWebGLAnimation(m_shapes)};
        util::writeStringToFile(pageSource, path.toStdString());
    }

    void saveRawImageDataButton() const
    {
        const QString path{common::ui::openSaveRawImageDataPathPickerDialog(q)};
        if(path.isEmpty()) {
            return;
        }

        const std::string bitmapData{geometrize::exporter::exportBitmapData(m_job->getCurrent())};
        util::writeStringToFile(bitmapData, path.toStdString());
    }

    void setShapes(const geometrize::ShapeTypes shapeTypes, const bool enable)
    {
        if(enable) {
            m_job->getPreferences().enableShapeTypes(shapeTypes);
        } else {
            m_job->getPreferences().disableShapeTypes(shapeTypes);
        }
    }

    void setScriptingModeEnabled(const bool enabled)
    {
        m_job->getPreferences().setScriptModeEnabled(enabled);

        if(enabled) {
            m_job->activateScriptedShapeMutation();
        } else {
            m_job->activateLibraryShapeMutation();
        }
    }

    void setShapeOpacity(const int opacity)
    {
        ui->shapeOpacityValueLabel->setText(QString::number(opacity));

        m_job->getPreferences().setShapeAlpha(opacity);
    }

    void setCandidateShapesPerStep(const int value)
    {
        ui->candidateShapesPerStepCountLabel->setText(QString::number(value));

        m_job->getPreferences().setCandidateShapeCount(value);
    }

    void setMutationsPerCandidateShape(const int value)
    {
        ui->mutationsPerCandidateShapeCountLabel->setText(QString::number(value));

        m_job->getPreferences().setMaxShapeMutations(value);
    }

    void setRandomSeed(const int value)
    {
        m_job->getPreferences().setSeed(value);
    }

    void setMaxThreads(const int value)
    {
        m_job->getPreferences().setMaxThreads(value);
    }

private:
    void setDisplayName(const QString& displayName)
    {
        q->setWindowTitle(tr("Geometrize - Image - %1").arg(displayName));
    }

    void updateWorkingImage()
    {
        const QPixmap pixmap{image::createPixmap(m_job->getCurrent())};
        m_scene.setWorkingPixmap(pixmap);
    }

    void setupOverlayImages()
    {
        const QPixmap target{image::createPixmap(m_job->getTarget())};
        m_scene.setTargetPixmap(target);
    }

    void syncUserInterfaceWithOptions()
    {
        geometrize::preferences::ImageJobPreferences& prefs{m_job->getPreferences()};

        const float startingTargetOpacity{10};
        ui->targetOpacitySlider->setValue(startingTargetOpacity);

        const geometrize::ImageRunnerOptions opts{prefs.getImageRunnerOptions()}; // Geometrize library options

        const auto usesShape = [&opts](const geometrize::ShapeTypes type) -> bool {
            const std::uint32_t shapes{static_cast<std::uint32_t>(opts.shapeTypes)};
            return shapes & type;
        };

        ui->usesRectangles->setChecked(usesShape(geometrize::RECTANGLE));
        ui->usesRotatedRectangles->setChecked(usesShape(geometrize::ROTATED_RECTANGLE));
        ui->usesTriangles->setChecked(usesShape(geometrize::TRIANGLE));
        ui->usesEllipses->setChecked(usesShape(geometrize::ELLIPSE));
        ui->usesRotatedEllipses->setChecked(usesShape(geometrize::ROTATED_ELLIPSE));
        ui->usesCircles->setChecked(usesShape(geometrize::CIRCLE));
        ui->usesLines->setChecked(usesShape(geometrize::LINE));
        ui->usesQuadraticBeziers->setChecked(usesShape(geometrize::QUADRATIC_BEZIER));
        ui->usesPolylines->setChecked(usesShape(geometrize::POLYLINE));

        ui->shapeOpacitySlider->setValue(opts.alpha);
        ui->candidateShapesPerStepSlider->setValue(opts.shapeCount);
        ui->mutationsPerCandidateShapeSlider->setValue(opts.maxShapeMutations);
        ui->randomSeedSpinBox->setValue(opts.seed);
        ui->maxThreadsSpinBox->setValue(opts.maxThreads);

        const bool scriptModeEnabled{prefs.isScriptModeEnabled()};
        ui->scriptingModeEnabledCheckbox->setChecked(scriptModeEnabled);

        const std::map<std::string, std::string> scripts{prefs.getScripts()};
        // TODO apply the scripts? or better use a signal from the prefs that something changed...? also need to subscribe to that for save actions on edit boxes on the UI itself
    }

    void setupScriptEditPanels()
    {
        // TODO set these up properly, pass a shape type, signals and slots etc
        for(const geometrize::ShapeTypes type : geometrize::allShapes) {
            ui->scriptingEditBoxes->layout()->addWidget(new ScriptEditorWidget(geometrize::strings::Strings::getShapeTypeNamePlural(type), nullptr)); // TODO add for named functions, not shape types?
        }
    }

    std::shared_ptr<job::ImageJob> m_job;
    ImageJobWindow* q;
    ImageJobScene m_scene;
    std::unique_ptr<Ui::ImageJobWindow> ui;

    std::unique_ptr<geometrize::Bitmap> m_initialJobImage;
    std::vector<geometrize::ShapeResult> m_shapes;

    bool m_running; // Whether the model is running (automatically)
};

ImageJobWindow::ImageJobWindow() :
    QMainWindow(nullptr),
    d{std::make_unique<ImageJobWindow::ImageJobWindowImpl>(this)}
{
}

ImageJobWindow::~ImageJobWindow()
{
}

void ImageJobWindow::setImageJob(const std::shared_ptr<job::ImageJob>& job)
{
    d->setImageJob(job);
}

void ImageJobWindow::on_actionExit_triggered()
{
    d->close();
}

void ImageJobWindow::on_actionLoad_Settings_Template_triggered()
{
    d->loadSettingsTemplate();
}

void ImageJobWindow::on_actionSave_Settings_Template_triggered()
{
    d->saveSettingsTemplate();
}

void ImageJobWindow::on_actionReveal_Launch_Window_triggered()
{
    d->revealLaunchWindow();
}

void ImageJobWindow::on_runStopButton_clicked()
{
    d->toggleRunning();
}

void ImageJobWindow::on_stepButton_clicked()
{
    d->stepModel();
}

void ImageJobWindow::on_clearButton_clicked()
{
    d->clearModel();
}

void ImageJobWindow::on_saveImageButton_clicked()
{
    d->saveImage();
}

void ImageJobWindow::on_previewSVGButton_clicked()
{
    d->previewSVG();
}

void ImageJobWindow::on_saveSVGButton_clicked()
{
    d->saveSVG();
}

void ImageJobWindow::on_saveRasterizedSVGButton_clicked()
{
    d->saveRasterizedSVG();
}

void ImageJobWindow::on_saveGeometryDataButton_clicked()
{
    d->saveGeometryData();
}

void ImageJobWindow::on_saveGIFButton_clicked()
{
    d->saveGIF();
}

void ImageJobWindow::on_saveCanvasAnimationButton_clicked()
{
    d->saveCanvasAnimation();
}

void ImageJobWindow::on_saveWebGLButton_clicked()
{
    d->saveWebGLAnimation();
}

void ImageJobWindow::on_saveRawImageDataButton_clicked()
{
    d->saveRawImageDataButton();
}

void ImageJobWindow::on_usesRectangles_clicked(bool checked)
{
    d->setShapes(geometrize::RECTANGLE, checked);
}

void ImageJobWindow::on_usesRotatedRectangles_clicked(bool checked)
{
    d->setShapes(geometrize::ROTATED_RECTANGLE, checked);
}

void ImageJobWindow::on_usesTriangles_clicked(bool checked)
{
    d->setShapes(geometrize::TRIANGLE, checked);
}

void ImageJobWindow::on_usesEllipses_clicked(bool checked)
{
    d->setShapes(geometrize::ELLIPSE, checked);
}

void ImageJobWindow::on_usesRotatedEllipses_clicked(bool checked)
{
    d->setShapes(geometrize::ROTATED_ELLIPSE, checked);
}

void ImageJobWindow::on_usesCircles_clicked(bool checked)
{
    d->setShapes(geometrize::CIRCLE, checked);
}

void ImageJobWindow::on_usesLines_clicked(bool checked)
{
    d->setShapes(geometrize::LINE, checked);
}

void ImageJobWindow::on_usesQuadraticBeziers_clicked(bool checked)
{
    d->setShapes(geometrize::QUADRATIC_BEZIER, checked);
}

void ImageJobWindow::on_usesPolylines_clicked(bool checked)
{
    d->setShapes(geometrize::POLYLINE, checked);
}

void ImageJobWindow::on_scriptingModeEnabledCheckbox_clicked(bool checked)
{
    d->setScriptingModeEnabled(checked);
}

void ImageJobWindow::on_shapeOpacitySlider_valueChanged(int value)
{
    d->setShapeOpacity(value);
}

void ImageJobWindow::on_candidateShapesPerStepSlider_valueChanged(int value)
{
    d->setCandidateShapesPerStep(value);
}

void ImageJobWindow::on_mutationsPerCandidateShapeSlider_valueChanged(int value)
{
    d->setMutationsPerCandidateShape(value);
}

void ImageJobWindow::on_randomSeedSpinBox_valueChanged(int value)
{
    d->setRandomSeed(value);
}

void ImageJobWindow::on_maxThreadsSpinBox_valueChanged(int value)
{
    d->setMaxThreads(value);
}

}

}
