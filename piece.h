#ifndef PIECE_H_
#define PIECE_H_

#include "finders.h"

static inline void rotPos(Pos3 bb0, Pos3 bb1, int *x, int *z, int rot)
{
    int posX, posZ;
    switch (rot)
    {
    case 0: posX = bb0.x + *x, posZ = bb1.z - *z; break;
    case 1: posX = bb0.x + *z, posZ = bb0.z + *x; break;
    case 2: posX = bb0.x + *x, posZ = bb0.z + *z; break;
    case 3: posX = bb1.x - *z, posZ = bb0.z + *x; break;
    default: UNREACHABLE();
    }
    *x = posX, *z = posZ;
}

#endif // PIECE_H_
