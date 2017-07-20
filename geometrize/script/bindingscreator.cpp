#include "script/bindingscreator.h"

#include <string>

#include <QImage>

#include "chaiscript/chaiscript.hpp"

#include "chaiscriptmathextras.h"

#include "geometrize/bitmap/bitmap.h"
#include "geometrize/model.h"
#include "geometrize/shape/circle.h"
#include "geometrize/shape/ellipse.h"
#include "geometrize/shape/line.h"
#include "geometrize/shape/polyline.h"
#include "geometrize/shape/quadraticbezier.h"
#include "geometrize/shape/rectangle.h"
#include "geometrize/shape/rotatedellipse.h"
#include "geometrize/shape/rotatedrectangle.h"
#include "geometrize/shape/triangle.h"

#include "bindingswrapper.h"
#include "dialog/launchwindow.h"
#include "exporter/imageexporter.h"
#include "job/imagejob.h"
#include "image/imageloader.h"

#define ADD_CONST_VAR(Class, Name) try { module->add(chaiscript::const_var(&Class::Name), #Name); } catch(...) { assert(0 && #Name); }
#define ADD_FREE_FUN(Name) try { module->add(chaiscript::fun(&Name), #Name); } catch(...) { assert(0 && #Name); }
#define ADD_MEMBER(Class, Name) try { module->add(chaiscript::fun(&Class::Name), #Name); } catch(...) { assert(0 && #Name); }
#define ADD_CONST_REF_MEMBER(Class, Name) try { module->add(chaiscript::fun([](const Class &r) -> decltype(auto) { return (r.Name); }), #Name); } catch(...) { assert(0 && #Name); }
#define ADD_TYPE(Class) try { module->add(chaiscript::user_type<Class>(), #Class); } catch(...) { assert(0 && #Class); }
#define ADD_BASE_CLASS(Base, Derived) try { module->add(chaiscript::base_class<Base, Derived>()); } catch (...) { assert(0 && #Base); }
#define ADD_CONSTRUCTOR(Class, Signature) try { module->add(chaiscript::constructor<Signature>(), #Class); } catch(...) { assert(0 && #Signature); }

namespace geometrize
{

namespace script
{

std::shared_ptr<chaiscript::Module> createDefaultBindings()
{
    auto module{std::make_shared<chaiscript::Module>()};

    ADD_FREE_FUN(debugBreak);
    ADD_FREE_FUN(printToConsole);

    ADD_FREE_FUN(directoryContainsFile);
    ADD_FREE_FUN(fileExists);
    ADD_FREE_FUN(directoryExists);
    ADD_FREE_FUN(readFileAsString);
    ADD_FREE_FUN(getFilePathsForDirectory);

    ADD_FREE_FUN(getScriptSearchPaths);
    ADD_FREE_FUN(getTemplateSearchPaths);

    ADD_FREE_FUN(getApplicationDirectoryPath);

    ADD_FREE_FUN(getFilesWithExtension);
    ADD_FREE_FUN(getFilesWithExtensions);
    ADD_FREE_FUN(getFirstFileWithExtension);
    ADD_FREE_FUN(getFirstFileWithExtensions);
    ADD_FREE_FUN(getScriptsForPath);

    ADD_FREE_FUN(getSupportedImageFileExtensions);

    ADD_FREE_FUN(openJob);

    ADD_FREE_FUN(openInDefaultApplication);
    ADD_FREE_FUN(revealInDefaultApplication);

    ADD_FREE_FUN(clearGlobalClipboard);
    ADD_FREE_FUN(getGlobalClipboardText);
    ADD_FREE_FUN(setGlobalClipboardText);

    ADD_FREE_FUN(stringBeginsWith);
    ADD_FREE_FUN(stringEndsWith);

    ADD_FREE_FUN(getAppDataLocation);

    ADD_FREE_FUN(writeStringToFile);

    ADD_FREE_FUN(percentEncode);

    ADD_FREE_FUN(randomInRange);
    ADD_FREE_FUN(clamp);

    ADD_FREE_FUN(split);

    ADD_FREE_FUN(setTranslatorsForLocale);

    return module;
}

std::shared_ptr<chaiscript::Module> createLaunchWindowBindings()
{
    using namespace geometrize::dialog;

    auto module{std::make_shared<chaiscript::Module>()};

    ADD_TYPE(LaunchWindow);

    ADD_BASE_CLASS(QMainWindow, LaunchWindow);
    ADD_BASE_CLASS(QWidget, LaunchWindow);

    ADD_CONSTRUCTOR(LaunchWindow, LaunchWindow());

    ADD_MEMBER(LaunchWindow, show);
    ADD_MEMBER(LaunchWindow, hide);

    return module;
}

std::shared_ptr<chaiscript::Module> createImageBindings()
{
    using namespace geometrize::image;

    auto module{std::make_shared<chaiscript::Module>()};

    ADD_TYPE(QImage);
    ADD_MEMBER(QImage, byteCount);
    ADD_MEMBER(QImage, isNull);
    ADD_MEMBER(QImage, width);
    ADD_MEMBER(QImage, height);

    ADD_FREE_FUN(loadImage);
    ADD_FREE_FUN(createBitmap);

    ADD_TYPE(Bitmap);

    return module;
}

std::shared_ptr<chaiscript::Module> createImageJobBindings()
{
    using namespace geometrize::job;

    auto module{std::make_shared<chaiscript::Module>()};

    ADD_TYPE(ImageJob);

    ADD_CONSTRUCTOR(ImageJob, ImageJob(Bitmap&));
    ADD_CONSTRUCTOR(ImageJob, ImageJob(Bitmap&, Bitmap&));

    ADD_MEMBER(ImageJob, getDisplayName);
    ADD_MEMBER(ImageJob, getJobId);
    ADD_MEMBER(ImageJob, stepModel);

    ADD_MEMBER(ImageJob, getTarget);
    ADD_MEMBER(ImageJob, getCurrent);

    //ShapeMutator& getShapeMutator();

    //geometrize::preferences::ImageJobPreferences& getPreferences();
    //void setPreferences(preferences::ImageJobPreferences preferences);

    return module;
}

std::shared_ptr<chaiscript::Module> createImageExportBindings()
{
    using namespace geometrize::exporter;

    auto module{std::make_shared<chaiscript::Module>()};

    ADD_FREE_FUN(exportBitmap);
    ADD_FREE_FUN(exportImage);

    return module;
}

std::shared_ptr<chaiscript::Module> createShapeMutationBindings()
{
    auto module{std::make_shared<chaiscript::Module>()};

    ADD_TYPE(Model);
    ADD_MEMBER(Model, getWidth);
    ADD_MEMBER(Model, getHeight);

    ADD_TYPE(Circle);
    ADD_CONST_REF_MEMBER(Circle, m_model);
    ADD_MEMBER(Circle, m_x);
    ADD_MEMBER(Circle, m_y);
    ADD_MEMBER(Circle, m_r);

    ADD_TYPE(Ellipse);
    ADD_CONST_REF_MEMBER(Ellipse, m_model);
    ADD_MEMBER(Ellipse, m_x);
    ADD_MEMBER(Ellipse, m_y);
    ADD_MEMBER(Ellipse, m_rx);
    ADD_MEMBER(Ellipse, m_ry);

    ADD_TYPE(Line);
    ADD_CONST_REF_MEMBER(Line, m_model);
    ADD_MEMBER(Line, m_x1);
    ADD_MEMBER(Line, m_y1);
    ADD_MEMBER(Line, m_x2);
    ADD_MEMBER(Line, m_y2);

    ADD_TYPE(Polyline);
    ADD_CONST_REF_MEMBER(Polyline, m_model);
    ADD_MEMBER(Polyline, m_points);

    ADD_TYPE(QuadraticBezier);
    ADD_CONST_REF_MEMBER(QuadraticBezier, m_model);
    ADD_MEMBER(QuadraticBezier, m_cx);
    ADD_MEMBER(QuadraticBezier, m_cy);
    ADD_MEMBER(QuadraticBezier, m_x1);
    ADD_MEMBER(QuadraticBezier, m_y1);
    ADD_MEMBER(QuadraticBezier, m_x2);
    ADD_MEMBER(QuadraticBezier, m_y2);

    ADD_TYPE(Rectangle);
    ADD_CONST_REF_MEMBER(Rectangle, m_model);
    ADD_MEMBER(Rectangle, m_x1);
    ADD_MEMBER(Rectangle, m_y1);
    ADD_MEMBER(Rectangle, m_x2);
    ADD_MEMBER(Rectangle, m_y2);

    ADD_TYPE(RotatedEllipse);
    ADD_CONST_REF_MEMBER(RotatedEllipse, m_model);
    ADD_MEMBER(RotatedEllipse, m_x);
    ADD_MEMBER(RotatedEllipse, m_y);
    ADD_MEMBER(RotatedEllipse, m_rx);
    ADD_MEMBER(RotatedEllipse, m_ry);
    ADD_MEMBER(RotatedEllipse, m_angle);

    ADD_TYPE(RotatedRectangle);
    ADD_CONST_REF_MEMBER(RotatedRectangle, m_model);
    ADD_MEMBER(RotatedRectangle, m_x1);
    ADD_MEMBER(RotatedRectangle, m_y1);
    ADD_MEMBER(RotatedRectangle, m_x2);
    ADD_MEMBER(RotatedRectangle, m_y2);
    ADD_MEMBER(RotatedRectangle, m_angle);

    ADD_TYPE(Triangle);
    ADD_CONST_REF_MEMBER(Triangle, m_model);
    ADD_MEMBER(Triangle, m_x1);
    ADD_MEMBER(Triangle, m_y1);
    ADD_MEMBER(Triangle, m_x2);
    ADD_MEMBER(Triangle, m_y2);
    ADD_MEMBER(Triangle, m_x3);
    ADD_MEMBER(Triangle, m_y3);

    return module;
}

std::shared_ptr<chaiscript::Module> createMathBindings()
{
    return chaiscript::extras::math::bootstrap();
}

}

}
