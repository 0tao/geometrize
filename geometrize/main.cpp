#include "dialog/launchwindow.h"

#include <functional>
#include <string>

#include <QApplication>
#include <QLocale>
#include <QString>
#include <QStringList>

#include "analytics/analyticswrapper.h"
#include "cli/commandlineparser.h"
#include "common/uiactions.h"
#include "dialog/launchwindow.h"
#include "dialog/welcomewindow.h"
#include "localization/localization.h"
#include "logger/logmessagehandlers.h"
#include "preferences/globalpreferences.h"
#include "version/versioninfo.h"

namespace {

void setApplicationSettingsFields()
{
    // Do not modify these
    const QString ORGANIZATION_NAME{"Sam Twidale"}; // The development organization's name.
    const QString ORGANIZATION_DOMAIN{"samcodes.co.uk"}; // The development organization's website domain.
    const QString APPLICATION_NAME{"Geometrize"}; // The application name.
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    QCoreApplication::setApplicationVersion(geometrize::version::getApplicationVersionString()); // This can change
}

void installMessageHandlers()
{
    qInstallMessageHandler(geometrize::log::handleLogMessages);
}

void setLocale(const QStringList& arguments)
{
    const std::string languageCode = [&arguments]() {
        const std::string overrideCode{geometrize::cli::getOverrideLocaleCode(arguments)};
        if(!overrideCode.empty()) {
            return overrideCode;
        }

        const auto& prefs{geometrize::preferences::getGlobalPreferences()};
        return prefs.getLanguageIsoCode();
    }();

    // TODO should only use this if a locale is set (by default there should be no preference, so it should use the system?)
    QLocale::setDefault(QLocale(QString::fromStdString(languageCode)));
    geometrize::setTranslatorsForLocale(QString::fromStdString(languageCode));
}

void setupAnalytics()
{
    geometrize::analytics::AnalyticsWrapper analytics;
    analytics.startSession();
    analytics.onLaunch();
}

int runAppConsoleMode(QApplication& app)
{
    return geometrize::cli::runApp(app);
}

int runAppGuiMode(QApplication& app)
{
	const auto& prefs{ geometrize::preferences::getGlobalPreferences() };
	if (prefs.shouldShowWelcomeScreenOnLaunch()) {
        geometrize::common::ui::openWelcomePage(); // Opens launch window on close
    } else {
        geometrize::common::ui::openLaunchWindow();
    }

    return app.exec();
}

std::function<int(QApplication&)> resolveLaunchFunction(const QStringList& arguments)
{
    if(geometrize::cli::shouldRunInConsoleMode(arguments)) {
        return ::runAppConsoleMode;
    }
    return ::runAppGuiMode;
}

}

int main(int argc, char* argv[])
{
    setApplicationSettingsFields();
    installMessageHandlers();

    QApplication app(argc, argv);

    setupAnalytics();

    const QStringList arguments{app.arguments()};
    setLocale(arguments);

    const auto run{resolveLaunchFunction(arguments)};
    return run(app);
}
