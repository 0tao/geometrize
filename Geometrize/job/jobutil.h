#pragma once

#include <string>

#include "templatemanifest.h"

class QStringList;

namespace chaiscript
{
class ChaiScript;
}

namespace geometrize
{

namespace util
{

void openJobs(const QStringList& urls);

bool openTemplate(chaiscript::ChaiScript& engine, const std::string& templateFolder);

TemplateManifest getTemplateManifest(const std::string& templateFolder);

}

}
