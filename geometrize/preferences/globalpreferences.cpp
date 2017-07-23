#include "globalpreferences.h"

#include <assert.h>
#include <fstream>
#include <ostream>

#include <QCoreApplication>

#include "cereal/archives/json.hpp"

#include "common/util.h"
#include "serialization/globalpreferencesdata.h"

namespace geometrize
{

namespace preferences
{

std::string getGlobalPreferencesConfigPath()
{
    return QCoreApplication::applicationDirPath().toStdString().append("/config/global_preferences.json");
}

class GlobalPreferences::GlobalPreferencesImpl
{
public:
    GlobalPreferencesImpl(const std::string& filePath)
    {
        load(filePath);
    }
    ~GlobalPreferencesImpl() = default;
    GlobalPreferencesImpl& operator=(const GlobalPreferencesImpl&) = default;
    GlobalPreferencesImpl(const GlobalPreferencesImpl&) = default;

    void load(const std::string& filePath)
    {
        std::ifstream input(filePath);
        try {
            m_data.archive(cereal::JSONInputArchive(input), m_imageJobResizeEnabled, m_imageJobResizeThreshold, m_languageIsoCode, m_scriptIsoCode, m_countryIsoCode);
        } catch(...) {
            assert(0 && "Failed to read global preferences");
        }
    }

    void save(const std::string& filePath)
    {
        std::ofstream output(filePath);
        try {
            m_data.archive(cereal::JSONOutputArchive(output), m_imageJobResizeEnabled, m_imageJobResizeThreshold, m_languageIsoCode, m_scriptIsoCode, m_countryIsoCode);
        } catch(...) {
            assert(0 && "Failed to write global preferences");
        }
    }

    void setImageJobResizeThreshold(const std::uint32_t width, const std::uint32_t height)
    {
        m_imageJobResizeThreshold.first = width;
        m_imageJobResizeThreshold.second = height;
    }

    std::pair<std::uint32_t, std::uint32_t> getImageJobResizeThreshold() const
    {
        return m_imageJobResizeThreshold;
    }

    bool isImageJobImageResizeEnabled() const
    {
        return m_imageJobResizeEnabled;
    }

    void setImageJobImageResizeEnabled(const bool enabled)
    {
        m_imageJobResizeEnabled = enabled;
    }

    std::string getLanguageIsoCode() const
    {
        return m_languageIsoCode;
    }

    void setLanguageIsoCode(const std::string& languageIsoCode)
    {
        m_languageIsoCode = languageIsoCode;
    }

    std::string getScriptIsoCode() const
    {
        return m_scriptIsoCode;
    }

    void setScriptIsoCode(const std::string& scriptIsoCode)
    {
        m_scriptIsoCode = scriptIsoCode;
    }

    std::string getCountryIsoCode() const
    {
        return m_countryIsoCode;
    }

    void setCountryIsoCode(const std::string& countryIsoCode)
    {
        m_countryIsoCode = countryIsoCode;
    }

    std::string getLocaleName() const
    {
        std::string localeName = m_languageIsoCode;
        if(!m_scriptIsoCode.empty()) {
            localeName += "_" + m_scriptIsoCode;
        }
        if(!m_countryIsoCode.empty()) {
            localeName += "_" + m_countryIsoCode;
        }
        return localeName;
    }

    void setLocaleName(const std::string& localeName)
    {
        const std::vector<std::string> components{geometrize::util::split(localeName, '_')};
        const std::size_t size{components.size()};

        std::string languageCode{""};
        std::string scriptCode{""};
        std::string countryCode{""};

        switch(size) {
        case 1:
            languageCode = components[0];
            break;
        case 2:
            languageCode = components[0];
            if(components[1].size() == 2) {
                countryCode = components[1];
            } else if(components[1].size() == 4) {
                scriptCode = components[1];
            } else {
                assert(0 && "First locale component was invalid");
            }
            break;
        case 3:
            languageCode = components[0];
            scriptCode = components[1];
            countryCode = components[2];
            break;
        default:
            assert(0 && "Failed to set invalid locale name");
            return;
        }

        setLanguageIsoCode(languageCode);
        setScriptIsoCode(scriptCode);
        setCountryIsoCode(countryCode);
    }

private:
    serialization::GlobalPreferencesData m_data;

    bool m_imageJobResizeEnabled{false};
    std::pair<std::uint32_t, std::uint32_t> m_imageJobResizeThreshold{512, 512};

    std::string m_languageIsoCode{"en"};
    std::string m_scriptIsoCode{"Latn"};
    std::string m_countryIsoCode{"US"};
};

GlobalPreferences::GlobalPreferences(const std::string& preferencesFilePath) : d{std::make_unique<GlobalPreferences::GlobalPreferencesImpl>(preferencesFilePath)}
{
}

GlobalPreferences::~GlobalPreferences()
{
}

void GlobalPreferences::load(const std::string& filePath)
{
    d->load(filePath);
}

void GlobalPreferences::save(const std::string& filePath)
{
    d->save(filePath);
}

bool GlobalPreferences::isImageJobImageResizeEnabled() const
{
    return d->isImageJobImageResizeEnabled();
}

void GlobalPreferences::setImageJobImageResizeEnabled(const bool enabled)
{
    d->setImageJobImageResizeEnabled(enabled);
}

std::pair<std::uint32_t, std::uint32_t> GlobalPreferences::getImageJobResizeThreshold() const
{
    return d->getImageJobResizeThreshold();
}

void GlobalPreferences::setImageJobResizeThreshold(const std::uint32_t width, const std::uint32_t height)
{
    d->setImageJobResizeThreshold(width, height);
}

std::string GlobalPreferences::getLanguageIsoCode() const
{
    return d->getLanguageIsoCode();
}

void GlobalPreferences::setLanguageIsoCode(const std::string& languageIsoCode)
{
    d->setLanguageIsoCode(languageIsoCode);
}

std::string GlobalPreferences::getScriptIsoCode() const
{
    return d->getScriptIsoCode();
}

void GlobalPreferences::setScriptIsoCode(const std::string& scriptIsoCode)
{
    d->setScriptIsoCode(scriptIsoCode);
}

std::string GlobalPreferences::getCountryIsoCode() const
{
    return d->getCountryIsoCode();
}

void GlobalPreferences::setCountryIsoCode(const std::string& countryIsoCode)
{
    d->setCountryIsoCode(countryIsoCode);
}

std::string GlobalPreferences::getLocaleName() const
{
    return d->getLocaleName();
}

void GlobalPreferences::setLocaleName(const std::string& localeName)
{
    d->setLocaleName(localeName);
}

}

}
