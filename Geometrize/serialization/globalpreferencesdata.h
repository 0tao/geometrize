#pragma once

#include <string>
#include <utility>

#include "cereal/cereal.hpp"
#include "cereal/types/utility.hpp"

namespace geometrize
{

namespace serialization
{

/**
 * @brief The GlobalPreferencesData struct represents the data for the global application preferences.
 */
class GlobalPreferencesData
{
public:
    template<class Archive>
    void archive(Archive& ar, bool& imageJobImageResizeEnabled, std::pair<std::uint32_t, std::uint32_t>& imageJobImageResizeThreshold)
    {
        ar(cereal::make_nvp(imageJobImageResizeEnabledKey, imageJobImageResizeEnabled));
        ar(cereal::make_nvp(imageJobImageResizeThresholdKey, imageJobImageResizeThreshold));
    }

private:
    const std::string imageJobImageResizeEnabledKey{"imageJobImageResizeEnabled"};
    const std::string imageJobImageResizeThresholdKey{"imageJobImageResizeThreshold"};
};

}

}
