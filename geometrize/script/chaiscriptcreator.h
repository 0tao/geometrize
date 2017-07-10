#pragma once

#include <memory>

namespace chaiscript
{
class ChaiScript;
}

namespace geometrize
{

namespace script
{

/**
 * @brief createChaiScript Creates a Chaiscript engine object.
 * @return The Chaiscript engine object.
 */
std::unique_ptr<chaiscript::ChaiScript> createChaiScript();

/**
 * @brief createChaiScriptShapeMutator Creates a Chaiscript engine object for mutating shapes.
 * @return The Chaiscript engine object.
 */
std::unique_ptr<chaiscript::ChaiScript> createChaiScriptShapeMutator();

}

}
