#include "state.h"
#include "grid.h"

void State::swapFillDirection()
{
    fillDirection =
        fillDirection == ACROSS ? DOWN : ACROSS;
}
