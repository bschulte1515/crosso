#include "grid.h"


QRect Cell::toRect()
{
    return toRectWithOffset(0, 0);
}

QRect Cell::toRectWithOffset(int x_offset, int y_offset)
{
    return QRect(
        (x * (size + Grid::inner_line_width)) + Grid::inner_line_width + x_offset,
        (y * (size + Grid::inner_line_width)) + Grid::inner_line_width + y_offset,
        size,
        size
    );
}
