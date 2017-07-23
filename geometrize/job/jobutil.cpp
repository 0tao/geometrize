#include "jobutil.h"

#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QUrl>

#include "chaiscript/chaiscript.hpp"

#include "common/formatsupport.h"
#include "common/sharedapp.h"
#include "common/util.h"
#include "job/imagejob.h"
#include "job/imagejobcreator.h"
#include "network/completionhandlers.h"
#include "network/downloader.h"
#include "network/networkactions.h"
#include "recents/recentitems.h"
#include "script/scriptrunner.h"

namespace geometrize
{

namespace util
{

void openJobs(const QStringList& urls, const bool addToRecents)
{
    if(urls.empty()) {
        return;
    }

    if(addToRecents) {
        for(const QString& url : urls) {
            common::app::SharedApp::get().getRecentFiles().add(url, url);
        }
    }

    for(const QString& s : urls) {
        const QUrl url{QUrl::fromUserInput(s)};
        if(url.isLocalFile()) {
            geometrize::job::createImageJobAndWindow(url.toLocalFile().toStdString(), url.toLocalFile().toStdString());
            continue;
        }

        std::vector<std::string> imageExtensions{format::getReadableImageFileExtensions(true)};
        for(const std::string& ext : imageExtensions) {
            if(url.toString().endsWith(QString::fromStdString(ext), Qt::CaseInsensitive)) {
                network::downloadImage(url, network::completionhandlers::onImageDownloadComplete);
                continue;
            }
        }

        network::downloadWebpage(url, network::completionhandlers::onWebpageDownloadComplete);
    }
}

bool openTemplate(chaiscript::ChaiScript& engine, const std::string& templateFolder)
{
    const std::vector<std::string> scripts{util::getScriptsForPath(templateFolder)};

    if(scripts.empty()) {
        assert(0 && "Could not find script for template");
        return false;
    }

    const std::string script{util::readFileAsString(scripts.front())};
    engine.set_global(chaiscript::var(templateFolder), "templateDirectory");
    geometrize::script::runScript(script, engine);

    return true;
}

TemplateManifest getTemplateManifest(const std::string& templateFolder)
{
    return TemplateManifest(util::getFirstFileWithExtensions(templateFolder, format::getSupportedTemplateManifestFileExtensions()));
}

}

}
