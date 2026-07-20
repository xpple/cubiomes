#ifndef PIECE_H_
#define PIECE_H_

#include "../finders.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void rotPos(Pos3 bb0, Pos3 bb1, int *x, int *z, int rot) {
    int posX, posZ;
    switch (rot) {
    case 0: posX = bb0.x + *x, posZ = bb1.z - *z; break;
    case 1: posX = bb0.x + *z, posZ = bb0.z + *x; break;
    case 2: posX = bb0.x + *x, posZ = bb0.z + *z; break;
    case 3: posX = bb1.x - *z, posZ = bb0.z + *x; break;
    default: UNREACHABLE();
    }
    *x = posX, *z = posZ;
}

// checks if given block is in any of the masks
static inline int shellBlockIsAir(const uint8_t *airMask, const uint8_t *waterMask, const uint8_t *dungeonAirMask, const uint8_t *dungeonSolidMask, int cx, int cz, int x, int y, int z) {
    if (!airMask || y < 0 || y > 255) return 0;
    int lx = x - cx, lz = z - cz;
    if (lx < 0 || lx > 15 || lz < 0 || lz > 15) return 0;
    int idx = (y << 8) | (lz << 4) | lx;
    if (dungeonSolidMask && ((dungeonSolidMask[idx >> 3] >> (idx & 7)) & 1)) return 0;
    if (dungeonAirMask && ((dungeonAirMask[idx >> 3] >> (idx & 7)) & 1)) return 1;
    if (waterMask && ((waterMask[idx >> 3] >> (idx & 7)) & 1)) return 0;
    if ((airMask[idx >> 3] >> (idx & 7)) & 1) return y >= 11;
    return 0;
}

static void generateBox(Piece *p, int cx, int cz, int x0, int y0, int z0, int x1, int y1, int z1, int skipAir, RandomSource rnd, const uint8_t *airMask, const uint8_t *waterMask, const uint8_t *dungeonAirMask, const uint8_t *dungeonSolidMask) {
    if (!skipAir) {
        int w = x1 - x0 + 1;
        int d = z1 - z0 + 1;
        int h = y1 - y0 + 1;
        int skips = w * d * h;
        if (!(w == 1 || d == 1 || h == 1)) {
            skips -= (w - 2) * (d - 2) * (h - 2);
        }
        rnd.skipN(rnd.state, skips);
        return;
    }

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            for (int z = z0; z <= z1; z++) {
                int tx = x, tz = z;
                rotPos(p->bb0, p->bb1, &tx, &tz, p->rot);
                if (tx >= cx && tx < cx + 16 && tz >= cz && tz < cz + 16) {
                    if (y == y0 || y == y1 || x == x0 || x == x1 || z == z0 || z == z1) {
                        int wy = p->bb0.y + y;
                        if (!shellBlockIsAir(airMask, waterMask, dungeonAirMask, dungeonSolidMask, cx, cz, tx, wy, tz)) {
                            rnd.nextFloat(rnd.state);
                        }
                    }
                }
            }
        }
    }
}

ATTR(always_inline)
static inline void generateMaybeBox(int x0, int y0, int z0, int x1, int y1, int z1, RandomSource rnd) {
    rnd.skipN(rnd.state, (y1-y0+1) * (x1-x0+1) * (z1-z0+1));
}

ATTR(always_inline)
static inline void maybeGenerateBlock(RandomSource rnd) {
    rnd.nextFloat(rnd.state);
}

#ifdef __cplusplus
}
#endif

#endif //PIECE_H_
