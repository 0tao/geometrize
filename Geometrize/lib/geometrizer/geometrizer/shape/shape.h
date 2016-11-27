#ifndef SHAPE_H
#define SHAPE_H

#include <vector>

#include "../scanline.h"

namespace geometrize
{

/**
 * Interface for shape rasterization and manipulation.
 * @author Sam Twidale (http://samcodes.co.uk/)
 */
class Shape
{
public:
    /**
     * @brief rasterize Creates a raster scanline representation of the shape.
     * @return Raster scanlines representing the shape.
     */
    virtual std::vector<Scanline> rasterize() const = 0;

    /**
     * @brief mutate Modifies the shape a little, typically with a random component.
     * Used to improve the shape's fit in a bitmap (trial-and-error style).
     */
    virtual void mutate() = 0;
};

}

#endif // SHAPE_H
