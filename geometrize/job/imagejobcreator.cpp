#include "imagejobcreator.h"

#include <assert.h>

#include "geometrize/core.h"

#include "dialog/imagejobwindow.h"
#include "imagejob.h"
#include "common/sharedapp.h"
#include "joblookup.h"
#include "image/imageloader.h"

namespace geometrize
{

namespace job
{

ImageJob* createImageJob(const std::string& displayName, const std::string& jobUrl)
{
    const QImage image{image::loadImage(QString::fromStdString(jobUrl))};
    BitmapData* bitmapData(image::createBitmapData(image));

    if(bitmapData == nullptr) {
        assert(0 && "Failed to create bitmap data");
        return nullptr;
    }

    ImageJob* job{new ImageJob(displayName, jobUrl, *bitmapData)};
    common::app::SharedApp::get().getJobLookup().setImageJob(displayName, job);
    return job;
}

void createImageJobAndWindow(const std::string& displayName, const std::string& jobUrl)
{
    ImageJob* job{createImageJob(displayName, jobUrl)};

    dialog::ImageJobWindow* imageJobWindow{new dialog::ImageJobWindow()};
    imageJobWindow->setImageJob(job);
    imageJobWindow->show();
}

}

}
