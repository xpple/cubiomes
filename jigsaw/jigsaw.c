#include "jigsaw.h"
#include "jigsaw_1_16_1.h"
#include "jigsaw_1_16_5.h"
#include "jigsaw_1_18_2.h"
#include "jigsaw_1_19_2.h"
#include "jigsaw_1_19_4.h"
#include "jigsaw_1_20_6.h"
#include "jigsaw_1_21_4.h"
#include "jigsaw_1_21_8.h"
#include "jigsaw_1_21_11.h"
#include "../rng.h"
#include "../biomes.h"
#include "../finders.h"

#include <stdlib.h>
#include <string.h>

#define MAX_JIGS 64
#define MAX_CANDIDATES 2048

// down, up, north, south, west, east
static const int stepX[6] = { 0, 0,  0, 0, -1, 1 };
static const int stepY[6] = { -1, 1, 0, 0,  0, 0 };
static const int stepZ[6] = { 0, 0, -1, 1,  0, 0 };

// horizontal order is north, east, south, west
static const uint8_t rotDir[4][6] = {
    { JD_DOWN, JD_UP, JD_NORTH, JD_SOUTH, JD_WEST,  JD_EAST  },
    { JD_DOWN, JD_UP, JD_EAST,  JD_WEST,  JD_NORTH, JD_SOUTH },
    { JD_DOWN, JD_UP, JD_SOUTH, JD_NORTH, JD_EAST,  JD_WEST  },
    { JD_DOWN, JD_UP, JD_WEST,  JD_EAST,  JD_SOUTH, JD_NORTH },
};

static void transformPos(int rot, int x, int y, int z, int *ox, int *oy, int *oz) {
    *oy = y;
    switch (rot) {
    case JIGSAW_ROT_NONE:  *ox =  x; *oz =  z; break;
    case JIGSAW_ROT_CW90:  *ox = -z; *oz =  x; break;
    case JIGSAW_ROT_CW180: *ox = -x; *oz = -z; break;
    case JIGSAW_ROT_CCW90: *ox =  z; *oz = -x; break;
    }
}

static void templateBounds(const JigsawTemplateDef *t, int rot,
        int px, int py, int pz, int bb[6])
{
    int sx = t->sx, sy = t->sy, sz = t->sz;
    switch (rot) {
    case JIGSAW_ROT_NONE:
        bb[0] = 0;        bb[2] = 0;
        bb[3] = sx - 1;   bb[5] = sz - 1;
        break;
    case JIGSAW_ROT_CW90:
        bb[0] = -(sz - 1); bb[2] = 0;
        bb[3] = 0;         bb[5] = sx - 1;
        break;
    case JIGSAW_ROT_CW180:
        bb[0] = -(sx - 1); bb[2] = -(sz - 1);
        bb[3] = 0;         bb[5] = 0;
        break;
    default: /* CCW90 */
        bb[0] = 0;         bb[2] = -(sx - 1);
        bb[3] = sz - 1;    bb[5] = 0;
        break;
    }
    bb[1] = 0;
    bb[4] = sy - 1;
    bb[0] += px; bb[1] += py; bb[2] += pz;
    bb[3] += px; bb[4] += py; bb[5] += pz;
}

#define COLLECTIONS_SHUFFLE(arr, n, rng, T) do { \
    int i_; \
    for (i_ = (n); i_ > 1; i_--) { \
        int j_ = nextInt((rng), i_); \
        T tmp_ = (arr)[i_ - 1]; (arr)[i_ - 1] = (arr)[j_]; (arr)[j_] = tmp_; \
    } \
} while (0) // apparently this lets it work like its one line

#define FASTUTIL_SHUFFLE(arr, n, rng, T) do { \
    int i_; \
    for (i_ = (n); i_-- != 0; ) { \
        int p_ = nextInt((rng), i_ + 1); \
        T tmp_ = (arr)[i_]; (arr)[i_] = (arr)[p_]; (arr)[p_] = tmp_; \
    } \
} while (0)

typedef struct {
    double x0, y0, z0, x1, y1, z1;
} JBox;

typedef struct {
    JBox outer;
    JBox *sub;
    int nsub, cap;
} JShape;

static JBox boxOf(const int bb[6])
{
    JBox b;
    b.x0 = bb[0]; b.y0 = bb[1]; b.z0 = bb[2];
    b.x1 = bb[3] + 1; b.y1 = bb[4] + 1; b.z1 = bb[5] + 1;
    return b;
}

static int shapeFits(const JShape *s, const int bb[6])
{
    JBox d = boxOf(bb);
    int i;
    d.x0 += 0.25; d.y0 += 0.25; d.z0 += 0.25;
    d.x1 -= 0.25; d.y1 -= 0.25; d.z1 -= 0.25;
    if (d.x0 < s->outer.x0 || d.x1 > s->outer.x1 ||
        d.y0 < s->outer.y0 || d.y1 > s->outer.y1 ||
        d.z0 < s->outer.z0 || d.z1 > s->outer.z1)
        return 0;
    for (i = 0; i < s->nsub; i++) {
        const JBox *b = &s->sub[i];
        if (d.x0 < b->x1 && b->x0 < d.x1 &&
            d.y0 < b->y1 && b->y0 < d.y1 &&
            d.z0 < b->z1 && b->z0 < d.z1)
            return 0;
    }
    return 1;
}

static int shapeSubtract(JShape *s, const int bb[6])
{
    if (s->nsub == s->cap) {
        int cap = s->cap ? s->cap * 2 : 16;
        JBox *p = (JBox*) realloc(s->sub, cap * sizeof(JBox));
        if (!p)
            return -1;
        s->sub = p;
        s->cap = cap;
    }
    s->sub[s->nsub++] = boxOf(bb);
    return 0;
}

typedef struct {
    int wx, wy, wz;
    uint8_t front, top;
    const JigsawBlockDef *def;
} JigInst;

typedef struct {
    const JigsawData *jd;
    uint64_t *rng;
    int maxDepth;
    JigsawPiece *pieces;
    int npieces, maxPieces;
    JShape *shapes;
    int nshapes, maxShapes;
    int *qPiece, *qShape, *qTop;
    int qhead, qtail;
} Ctx;

static int shuffledJigsawBlocks(const Ctx *c, const JigsawTemplateDef *t,
        int rot, int bx, int by, int bz, JigInst *out)
{
    int n = t->jigsawCount, i;
    if (n > MAX_JIGS)
        return -1;
    for (i = 0; i < n; i++) {
        const JigsawBlockDef *d = &c->jd->jigsawBlocks[t->jigsawStart + i];
        JigInst *ji = &out[i];
        transformPos(rot, d->x, d->y, d->z, &ji->wx, &ji->wy, &ji->wz);
        ji->wx += bx; ji->wy += by; ji->wz += bz;
        ji->front = rotDir[rot][d->front];
        ji->top = rotDir[rot][d->top];
        ji->def = d;
    }
    COLLECTIONS_SHUFFLE(out, n, c->rng, JigInst);
    return n;
}

/* pool exists and is usable: non-empty or literally the "empty" pool */
static int poolOk(const JigsawData *jd, int poolIdx)
{
    if (poolIdx < 0)
        return 0;
    return jd->pools[poolIdx].totalWeight > 0 ||
           strcmp(jd->pools[poolIdx].name, "empty") == 0;
}

/* StructureTemplatePool.getShuffledTemplates: fastutil shuffle of the
 * weight-expanded template list; appends entry indices to cand */
static int appendShuffledPool(const Ctx *c, int poolIdx, int *cand, int ncand)
{
    const JigsawPoolDef *p = &c->jd->pools[poolIdx];
    int buf[MAX_CANDIDATES];
    int n = 0, i, w;
    if (p->totalWeight > MAX_CANDIDATES || ncand + p->totalWeight > MAX_CANDIDATES)
        return -1;
    for (i = 0; i < p->entryCount; i++) {
        const JigsawPoolEntryDef *e = &c->jd->poolEntries[p->entryStart + i];
        for (w = 0; w < e->weight; w++)
            buf[n++] = p->entryStart + i;
    }
    FASTUTIL_SHUFFLE(buf, n, c->rng, int);
    memcpy(cand + ncand, buf, n * sizeof(int));
    return ncand + n;
}

static int enqueue(Ctx *c, int pieceIdx, int shapeIdx, int boundsTop)
{
    c->qPiece[c->qtail] = pieceIdx;
    c->qShape[c->qtail] = shapeIdx;
    c->qTop[c->qtail] = boundsTop;
    c->qtail++;
    return 0;
}

static int tryPlacingChildren(Ctx *c, int pieceIdx, int shapeIdx,
        int boundsTop, int depth)
{
    const JigsawData *jd = c->jd;
    JigsawPiece parent = c->pieces[pieceIdx];
    const JigsawTemplateDef *ptmpl = &jd->templates[parent.templateIdx];
    int k = parent.bb[1];
    int interiorShape = -1;
    JigInst jigs[MAX_JIGS];
    int njigs, ji;

    njigs = shuffledJigsawBlocks(c, ptmpl, parent.rotation, parent.x, parent.y, parent.z, jigs);
    if (njigs < 0)
        return -1;

    for (ji = 0; ji < njigs; ji++) {
        const JigInst *jig = &jigs[ji];
        int attachX = jig->wx + stepX[jig->front];
        int attachY = jig->wy + stepY[jig->front];
        int attachZ = jig->wz + stepZ[jig->front];
        int l = jig->wy - k;
        int poolIdx = jig->def->poolIdx;
        int fbIdx, inside, useShape, n2;
        int cand[MAX_CANDIDATES];
        int ncand = 0, ci;

        if (!poolOk(jd, poolIdx))
            continue;
        fbIdx = jd->pools[poolIdx].fallbackIdx;
        if (fbIdx >= 0 && !poolOk(jd, fbIdx))
            continue;

        inside = attachX >= parent.bb[0] && attachX <= parent.bb[3] &&
                 attachY >= parent.bb[1] && attachY <= parent.bb[4] &&
                 attachZ >= parent.bb[2] && attachZ <= parent.bb[5];
        if (inside) {
            if (interiorShape < 0) {
                if (c->nshapes == c->maxShapes)
                    return -1;
                interiorShape = c->nshapes++;
                c->shapes[interiorShape].outer = boxOf(parent.bb);
                c->shapes[interiorShape].sub = NULL;
                c->shapes[interiorShape].nsub = 0;
                c->shapes[interiorShape].cap = 0;
            }
            useShape = interiorShape;
            n2 = k;
        } else {
            useShape = shapeIdx;
            n2 = boundsTop;
        }

        if (depth != c->maxDepth) {
            ncand = appendShuffledPool(c, poolIdx, cand, ncand);
            if (ncand < 0)
                return -1;
        }
        if (fbIdx >= 0) {
            ncand = appendShuffledPool(c, fbIdx, cand, ncand);
            if (ncand < 0)
                return -1;
        }

        for (ci = 0; ci < ncand; ci++) {
            const JigsawPoolEntryDef *e = &jd->poolEntries[cand[ci]];
            const JigsawTemplateDef *ctmpl;
            uint8_t rots[4] = { 0, 1, 2, 3 };
            int ri;

            if (e->kind == JIGSAW_ELEMENT_EMPTY)
                break;
            if (e->kind != JIGSAW_ELEMENT_SINGLE &&
                e->kind != JIGSAW_ELEMENT_LEGACY_SINGLE)
                return -1; // feature/list elements not supported yet
            ctmpl = &jd->templates[e->templateIdx];

            COLLECTIONS_SHUFFLE(rots, 4, c->rng, uint8_t);
            for (ri = 0; ri < 4; ri++) {
                int rot2 = rots[ri];
                JigInst cjigs[MAX_JIGS];
                int ncj, cj;
                ncj = shuffledJigsawBlocks(c, ctmpl, rot2, 0, 0, 0, cjigs);
                if (ncj < 0)
                    return -1;
                for (cj = 0; cj < ncj; cj++) {
                    const JigInst *cjig = &cjigs[cj];
                    int ax, ay, az, bb3[6], p, q, r, s, t;
                    JigsawPiece *child;
                    /* JigsawBlock.canAttach */
                    if (cjig->front != (jig->front ^ 1))
                        continue;
                    if (jig->def->joint != JIGSAW_JOINT_ROLLABLE &&
                        jig->top != cjig->top)
                        continue;
                    if (jig->def->target != cjig->def->name)
                        continue;

                    ax = attachX - cjig->wx;
                    ay = attachY - cjig->wy;
                    az = attachZ - cjig->wz;
                    templateBounds(ctmpl, rot2, ax, ay, az, bb3);
                    p = bb3[1];
                    q = cjig->wy;
                    r = l - q + stepY[jig->front];
                    s = k + r;
                    t = s - p;
                    bb3[1] += t; bb3[4] += t;
                    ay += t;

                    if (!shapeFits(&c->shapes[useShape], bb3))
                        continue;
                    if (shapeSubtract(&c->shapes[useShape], bb3))
                        return -1;

                    if (c->npieces == c->maxPieces)
                        return -1;
                    child = &c->pieces[c->npieces++];
                    child->templateIdx = (int16_t)(ctmpl - jd->templates);
                    child->rotation = (uint8_t)rot2;
                    child->depth = (uint8_t)(depth + 1);
                    child->groundLevelDelta =
                        (int16_t)(parent.groundLevelDelta - r);
                    child->x = ax; child->y = ay; child->z = az;
                    memcpy(child->bb, bb3, sizeof(bb3));

                    if (depth + 1 <= c->maxDepth)
                        enqueue(c, c->npieces - 1, useShape, n2);
                    goto next_jig;
                }
            }
        }
        next_jig:;
    }
    return 0;
}

int getJigsawPieces(const JigsawData *jd, const JigsawConfig *jc,
        const char *startPool, uint64_t *rng, int blockX, int blockZ,
        JigsawPiece *out, int maxOut)
{
    Ctx c;
    const JigsawPoolDef *pool = NULL;
    const JigsawPoolEntryDef *startEntry = NULL;
    const JigsawTemplateDef *stmpl;
    JigsawPiece *start;
    int i, rot, pick, cx, cz, k, l, dy, dist, ret = -1;

    if (jc->useExpansionHack || jc->projectStartToHeightmap ||
        jc->startJigsawName)
        return -1; // not implemented yet

    for (i = 0; i < jd->poolCount; i++) {
        if (strcmp(jd->pools[i].name, startPool) == 0) {
            pool = &jd->pools[i];
            break;
        }
    }
    if (!pool || pool->totalWeight == 0 || maxOut < 1)
        return -1;

    rot = nextInt(rng, 4);

    pick = nextInt(rng, pool->totalWeight);
    for (i = 0; i < pool->entryCount; i++) {
        const JigsawPoolEntryDef *e = &jd->poolEntries[pool->entryStart + i];
        if (pick < e->weight) {
            startEntry = e;
            break;
        }
        pick -= e->weight;
    }
    if (!startEntry || (startEntry->kind != JIGSAW_ELEMENT_SINGLE &&
                        startEntry->kind != JIGSAW_ELEMENT_LEGACY_SINGLE))
        return -1;
    stmpl = &jd->templates[startEntry->templateIdx];

    start = &out[0];
    start->templateIdx = startEntry->templateIdx;
    start->rotation = (uint8_t)rot;
    start->depth = 0;
    start->groundLevelDelta = 1;
    start->x = blockX; start->y = jc->startHeight; start->z = blockZ;
    templateBounds(stmpl, rot, blockX, jc->startHeight, blockZ, start->bb);

    cx = (start->bb[0] + start->bb[3]) / 2;
    cz = (start->bb[2] + start->bb[5]) / 2;
    k = jc->startHeight;
    l = start->bb[1] + start->groundLevelDelta;
    dy = k - l;
    start->y += dy; start->bb[1] += dy; start->bb[4] += dy;

    if (jc->maxDepth <= 0)
        return 1;

    memset(&c, 0, sizeof(c));
    c.jd = jd;
    c.rng = rng;
    c.maxDepth = jc->maxDepth;
    c.pieces = out;
    c.npieces = 1;
    c.maxPieces = maxOut;
    c.maxShapes = maxOut + 1;
    c.shapes = (JShape*) calloc(c.maxShapes, sizeof(JShape));
    c.qPiece = (int*) malloc(maxOut * sizeof(int));
    c.qShape = (int*) malloc(maxOut * sizeof(int));
    c.qTop = (int*) malloc(maxOut * sizeof(int));
    if (!c.shapes || !c.qPiece || !c.qShape || !c.qTop)
        goto done;

    dist = jc->maxDistanceFromCenter;
    c.shapes[0].outer.x0 = cx - dist; c.shapes[0].outer.x1 = cx + dist + 1;
    c.shapes[0].outer.y0 = k - dist;  c.shapes[0].outer.y1 = k + dist + 1;
    c.shapes[0].outer.z0 = cz - dist; c.shapes[0].outer.z1 = cz + dist + 1;
    c.nshapes = 1;
    if (shapeSubtract(&c.shapes[0], start->bb))
        goto done;

    enqueue(&c, 0, 0, k + dist);
    while (c.qhead < c.qtail) {
        int pi = c.qPiece[c.qhead];
        int si = c.qShape[c.qhead];
        int bt = c.qTop[c.qhead];
        c.qhead++;
        if (tryPlacingChildren(&c, pi, si, bt, c.pieces[pi].depth))
            goto done;
    }
    ret = c.npieces;

done:
    for (i = 0; i < c.nshapes; i++)
        free(c.shapes[i].sub);
    free(c.shapes);
    free(c.qPiece);
    free(c.qShape);
    free(c.qTop);
    return ret;
}

int getJigsawConfig(int structureType, int mc, int biome, JigsawConfig *jc)
{
    memset(jc, 0, sizeof(*jc));
    jc->nStartPools = 1;
    jc->maxDistanceFromCenter = 80;

    switch (structureType) {
    case Bastion:
        if (mc < MC_1_16_1)
            return 0;
        jc->startHeight = 33;
        if (mc <= MC_1_16_1) {
            jc->startPools[0] = "bastion/units/base";
            jc->startPools[1] = "bastion/hoglin_stable/origin";
            jc->startPools[2] = "bastion/treasure/starters";
            jc->startPools[3] = "bastion/bridge/start";
            jc->nStartPools = 4;
            jc->maxDepth = 60;
        } else {
            jc->startPools[0] = "bastion/starts";
            jc->maxDepth = 6;
        }
        return 1;

    case Village:
        if (mc < MC_1_14)
            return 0;
        switch (biome) {
        case meadow:
        case plains:       jc->startPools[0] = "village/plains/town_centers";  break;
        case desert:       jc->startPools[0] = "village/desert/town_centers";  break;
        case savanna:      jc->startPools[0] = "village/savanna/town_centers"; break;
        case taiga:        jc->startPools[0] = "village/taiga/town_centers";   break;
        case snowy_tundra: jc->startPools[0] = "village/snowy/town_centers";   break;
        default:
            return 0;
        }
        jc->maxDepth = 6;
        jc->useExpansionHack = 1;
        jc->projectStartToHeightmap = 1;
        return 1;

    case Outpost:
        if (mc < MC_1_14)
            return 0;
        jc->startPools[0] = "pillager_outpost/base_plates";
        jc->maxDepth = 7;
        jc->useExpansionHack = 1;
        jc->projectStartToHeightmap = 1;
        return 1;

    case Ancient_City:
        if (mc < MC_1_19)
            return 0;
        jc->startPools[0] = "ancient_city/city_center";
        jc->startJigsawName = "city_anchor";
        jc->maxDepth = 7;
        jc->startHeight = -27;
        jc->maxDistanceFromCenter = 116;
        return 1;

    default:
        return 0;
    }
}

// if theres a version missing it uses the most recent one
static const struct { int minMc; const JigsawData *data; } jigsawTables[] = {
    { MC_1_21_11, &jigsaw_data_1_21_11 },
    { MC_1_21_9,  &jigsaw_data_1_21_8  },
    { MC_1_21_WD, &jigsaw_data_1_21_4  },
    { MC_1_20_6,  &jigsaw_data_1_20_6  },
    { MC_1_19_4,  &jigsaw_data_1_19_4  },
    { MC_1_19_2,  &jigsaw_data_1_19_2  },
    { MC_1_18_2,  &jigsaw_data_1_18_2  },
    { MC_1_16_5,  &jigsaw_data_1_16_5  },
    { MC_1_16_1,  &jigsaw_data_1_16_1  },
};

const JigsawData *getJigsawData(int mc)
{
    size_t i;
    for (i = 0; i < sizeof(jigsawTables) / sizeof(jigsawTables[0]); i++)
        if (mc >= jigsawTables[i].minMc)
            return jigsawTables[i].data;
    return NULL;
}

int getJigsawStructurePieces(int structureType, int mc, int biome,
        uint64_t seed, int chunkX, int chunkZ, JigsawPiece *out, int maxOut)
{
    const JigsawData *jd = getJigsawData(mc);
    JigsawConfig jc;
    uint64_t rng, l1, l2;
    const char *startPool;

    if (!jd || !getJigsawConfig(structureType, mc, biome, &jc))
        return -1;
    if (mc >= MC_1_20_6) // TODO add priority stuff for newer version
        return -1;

    setSeed(&rng, seed);
    l1 = nextLong(&rng);
    l2 = nextLong(&rng);
    setSeed(&rng, ((uint64_t)(int64_t)chunkX * l1) ^ ((uint64_t)(int64_t)chunkZ * l2) ^ seed);

    startPool = jc.startPools[0];
    if (jc.nStartPools > 1)
        startPool = jc.startPools[nextInt(&rng, jc.nStartPools)];

    return getJigsawPieces(jd, &jc, startPool, &rng, chunkX * 16, chunkZ * 16, out, maxOut);
}

void getJigsawContainerPos(const JigsawData *jd, const JigsawPiece *p, int c, int *x, int *y, int *z)
{
    const JigsawTemplateDef *t = &jd->templates[p->templateIdx];
    const JigsawContainerDef *cd = &jd->containers[t->containerStart + c];
    transformPos(p->rotation, cd->x, cd->y, cd->z, x, y, z);
    *x += p->x; *y += p->y; *z += p->z;
}
