#pragma once

#include <memory>

namespace geometrize
{
class RecentItems;
}

namespace geometrize
{

namespace common
{

namespace app
{

/**
 * @brief The SharedApp class is a lazy singleton that contains common core functionality.
 */
class SharedApp
{
public:
    static SharedApp& get();
    SharedApp& operator=(const SharedApp&) = delete;
    SharedApp(const SharedApp&) = delete;

    /**
     * @brief getRecentFiles Gets a reference to the recent files list.
     * @return The recent files list.
     */
    RecentItems& getRecentFiles();

private:
    class SharedAppImpl;
    std::unique_ptr<SharedAppImpl> d;

    SharedApp();
    ~SharedApp();
};

}

}

}
