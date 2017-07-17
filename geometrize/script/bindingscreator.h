#pragma once

#include <memory>

namespace chaiscript
{
class Module;
}

namespace geometrize
{

namespace script
{

/**
 * @brief createDefaultBindings Creates the Chaiscript to C++ bindings.
 * @return A shared pointer to a module encapsulating the bindings.
 */
std::shared_ptr<chaiscript::Module> createDefaultBindings();

/**
 * @brief createShapeMutationBindings Creates the Chaiscript to C++ bindings relating to Geometrize shape setup and mutation.
 * @return A shared pointer to a module encapsulating the bindings.
 */
std::shared_ptr<chaiscript::Module> createShapeMutationBindings();

/**
 * @brief createMathBindings Creates the Chaiscript to C++ bindings for common math functions.
 * @return A shared pointer to a module encapsulating the bindings.
 */
std::shared_ptr<chaiscript::Module> createMathBindings();

}

}
