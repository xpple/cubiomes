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

static int findString(const JigsawData *jd, const char *s)
{
    int i;
    for (i = 0; i < jd->stringCount; i++)
        if (strcmp(jd->strings[i], s) == 0)
            return i;
    return -1;
}

static int findPool(const JigsawData *jd, const char *name)
{
    int i;
    for (i = 0; i < jd->poolCount; i++)
        if (strcmp(jd->pools[i].name, name) == 0)
            return i;
    return -1;
}

/* the template that carries a pool entry's jigsaw blocks, -1 if none */
static int elementTemplateIdx(const JigsawData *jd, const JigsawPoolEntryDef *e)
{
    switch (e->kind) {
    case JIGSAW_ELEMENT_SINGLE:
    case JIGSAW_ELEMENT_LEGACY_SINGLE:
        return e->templateIdx;
    case JIGSAW_ELEMENT_LIST: // a list takes its connectors from member 0
        return e->listCount ? jd->poolEntries[e->listStart].templateIdx : -1;
    default:
        return -1;
    }
}


static void elementBounds(const JigsawData *jd, const JigsawPoolEntryDef *e,
        int rot, int px, int py, int pz, int bb[6])
{
    int i;
    switch (e->kind) {
    case JIGSAW_ELEMENT_SINGLE:
    case JIGSAW_ELEMENT_LEGACY_SINGLE:
        templateBounds(&jd->templates[e->templateIdx], rot, px, py, pz, bb);
        return;
    case JIGSAW_ELEMENT_LIST:
        for (i = 0; i < e->listCount; i++) {
            int mb[6], k;
            elementBounds(jd, &jd->poolEntries[e->listStart + i], rot,
                          px, py, pz, mb);
            if (i == 0) {
                memcpy(bb, mb, 6 * sizeof(int));
                continue;
            }
            for (k = 0; k < 3; k++) {
                if (mb[k] < bb[k])
                    bb[k] = mb[k];
                if (mb[k+3] > bb[k+3])
                    bb[k+3] = mb[k+3];
            }
        }
        return;
    default:
        bb[0] = bb[3] = px;
        bb[1] = bb[4] = py;
        bb[2] = bb[5] = pz;
        return;
    }
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
    const Generator *g;
    const SurfaceNoise *sn;
    int doExpansionHack;
    int *poolMax;               // getMaxSize per pool, -1 until computed
    JigsawBlockDef featureBlock;
} Ctx;

static void makeFeatureBlock(const JigsawData *jd, JigsawBlockDef *b)
{
    int name = findString(jd, "bottom"), target = findString(jd, "empty");
    memset(b, 0, sizeof(*b));
    b->front = JD_DOWN;
    b->top = JD_SOUTH;
    b->joint = JIGSAW_JOINT_ROLLABLE;
    b->poolIdx = (int16_t) findPool(jd, "empty");
    b->name = (uint16_t)(name < 0 ? 0xffff : name);
    b->target = (uint16_t)(target < 0 ? 0xfffe : target);
}

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

// getShuffledJigsawBlocks
static int elementJigsawBlocks(const Ctx *c, const JigsawPoolEntryDef *e,
        int rot, int bx, int by, int bz, JigInst *out)
{
    switch (e->kind) {
    case JIGSAW_ELEMENT_SINGLE:
    case JIGSAW_ELEMENT_LEGACY_SINGLE:
        return shuffledJigsawBlocks(c, &c->jd->templates[e->templateIdx],
                                    rot, bx, by, bz, out);
    case JIGSAW_ELEMENT_LIST:
        if (e->listCount == 0)
            return 0;
        return elementJigsawBlocks(c, &c->jd->poolEntries[e->listStart],
                                   rot, bx, by, bz, out);
    case JIGSAW_ELEMENT_FEATURE:
        out[0].wx = bx; out[0].wy = by; out[0].wz = bz;
        out[0].front = JD_DOWN;
        out[0].top = JD_SOUTH;
        out[0].def = &c->featureBlock;
        return 1;
    default:
        return 0;
    }
}

// getMaxSize
static int poolMaxSize(Ctx *c, int poolIdx)
{
    const JigsawPoolDef *p;
    int i, mx = 0;

    if (poolIdx < 0)
        return 0;
    if (c->poolMax[poolIdx] >= 0)
        return c->poolMax[poolIdx];

    p = &c->jd->pools[poolIdx];
    for (i = 0; i < p->entryCount; i++) {
        const JigsawPoolEntryDef *e = &c->jd->poolEntries[p->entryStart + i];
        int bb[6], span;
        if (e->weight == 0)
            continue;
        if (e->kind == JIGSAW_ELEMENT_EMPTY) {
            span = 2;
        } else {
            elementBounds(c->jd, e, JIGSAW_ROT_NONE, 0, 0, 0, bb);
            span = bb[4] - bb[1] + 1;
        }
        if (span > mx)
            mx = span;
    }
    c->poolMax[poolIdx] = mx;
    return mx;
}

// pool exists and is usable
static int poolOk(const JigsawData *jd, int poolIdx)
{
    if (poolIdx < 0)
        return 0;
    return jd->pools[poolIdx].totalWeight > 0 ||
           strcmp(jd->pools[poolIdx].name, "empty") == 0;
}

// getShuffledTemplates
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
    const JigsawPoolEntryDef *pentry = &jd->poolEntries[parent.entryIdx];
    int parentRigid = parent.projection == JIGSAW_PROJECTION_RIGID;
    int k = parent.bb[1];
    int interiorShape = -1;
    JigInst jigs[MAX_JIGS];
    int njigs, ji;

    njigs = elementJigsawBlocks(c, pentry, parent.rotation,
                                parent.x, parent.y, parent.z, jigs);
    if (njigs < 0)
        return -1;

    for (ji = 0; ji < njigs; ji++) {
        const JigInst *jig = &jigs[ji];
        int attachX = jig->wx + stepX[jig->front];
        int attachY = jig->wy + stepY[jig->front];
        int attachZ = jig->wz + stepZ[jig->front];
        int l = jig->wy - k;
        int poolIdx = jig->def->poolIdx;
        int surfaceY = -1; // heightmap under this connector, sampled at most once
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
            uint8_t rots[4] = { 0, 1, 2, 3 };
            int childRigid, ri;

            if (e->kind == JIGSAW_ELEMENT_EMPTY)
                break;
            childRigid = e->projection == JIGSAW_PROJECTION_RIGID;

            COLLECTIONS_SHUFFLE(rots, 4, c->rng, uint8_t);
            for (ri = 0; ri < 4; ri++) {
                int rot2 = rots[ri];
                JigInst cjigs[MAX_JIGS];
                int bb2[6], grow = 0;
                int ncj, cj;
                ncj = elementJigsawBlocks(c, e, rot2, 0, 0, 0, cjigs);
                if (ncj < 0)
                    return -1;
                elementBounds(jd, e, rot2, 0, 0, 0, bb2);

                // the expansion hack: a short piece whose own connectors
                // point back into itself gets its box stretched to fit
                // whatever those connectors could still pull in
                if (c->doExpansionHack && bb2[4] - bb2[1] + 1 <= 16) {
                    for (cj = 0; cj < ncj; cj++) {
                        const JigInst *cjig = &cjigs[cj];
                        int rx = cjig->wx + stepX[cjig->front];
                        int ry = cjig->wy + stepY[cjig->front];
                        int rz = cjig->wz + stepZ[cjig->front];
                        int cpool, a, b;
                        if (rx < bb2[0] || rx > bb2[3] || ry < bb2[1] ||
                            ry > bb2[4] || rz < bb2[2] || rz > bb2[5])
                            continue;
                        cpool = cjig->def->poolIdx;
                        a = poolMaxSize(c, cpool);
                        b = cpool >= 0 ? poolMaxSize(c, jd->pools[cpool].fallbackIdx) : 0;
                        if (b > a)
                            a = b;
                        if (a > grow)
                            grow = a;
                    }
                }

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
                    elementBounds(jd, e, rot2, ax, ay, az, bb3);
                    p = bb3[1];
                    q = cjig->wy;
                    r = l - q + stepY[jig->front];
                    if (parentRigid && childRigid) {
                        s = k + r;
                    } else { // either end floats, so sit the joint on the ground
                        if (surfaceY < 0) {
                            if (!c->g || !c->sn)
                                return -1;
                            surfaceY = getSingleBlockSurfaceHeight(c->g, c->sn,
                                                        jig->wx, jig->wz, 0);
                        }
                        s = surfaceY - q;
                    }
                    t = s - p;
                    bb3[1] += t; bb3[4] += t;
                    ay += t;
                    if (grow > 0) {
                        int span = bb3[4] - bb3[1];
                        if (grow + 1 > span)
                            span = grow + 1;
                        bb3[4] = bb3[1] + span;
                    }

                    if (!shapeFits(&c->shapes[useShape], bb3))
                        continue;
                    if (shapeSubtract(&c->shapes[useShape], bb3))
                        return -1;

                    if (c->npieces == c->maxPieces)
                        return -1;
                    child = &c->pieces[c->npieces++];
                    child->templateIdx = (int16_t) elementTemplateIdx(jd, e);
                    child->entryIdx = (int16_t) cand[ci];
                    child->rotation = (uint8_t)rot2;
                    child->depth = (uint8_t)(depth + 1);
                    child->projection = e->projection;
                    child->groundLevelDelta = childRigid
                        ? (int16_t)(parent.groundLevelDelta - r) : 1;
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
        const Generator *g, const SurfaceNoise *sn,
        JigsawPiece *out, int maxOut)
{
    Ctx c;
    const JigsawPoolDef *pool = NULL;
    const JigsawPoolEntryDef *startEntry = NULL;
    JigsawPiece *start;
    int i, rot, pick, cx, cz, k, l, dy, dist, ret = -1;

    if (jc->startJigsawName)
        return -1; // not implemented yet
    if (jc->projectStartToHeightmap && (!g || !sn))
        return -1;

    i = findPool(jd, startPool);
    if (i < 0)
        return -1;
    pool = &jd->pools[i];
    if (pool->totalWeight == 0 || maxOut < 1)
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
    if (!startEntry || startEntry->kind == JIGSAW_ELEMENT_EMPTY)
        return -1;

    start = &out[0];
    start->templateIdx = (int16_t) elementTemplateIdx(jd, startEntry);
    start->entryIdx = (int16_t)(startEntry - jd->poolEntries);
    start->rotation = (uint8_t)rot;
    start->depth = 0;
    start->projection = startEntry->projection;
    start->groundLevelDelta = 1;
    start->x = blockX; start->y = jc->startHeight; start->z = blockZ;
    elementBounds(jd, startEntry, rot, blockX, jc->startHeight, blockZ, start->bb);

    cx = (start->bb[0] + start->bb[3]) / 2;
    cz = (start->bb[2] + start->bb[5]) / 2;
    k = jc->startHeight;
    if (jc->projectStartToHeightmap)
        k += getSingleBlockSurfaceHeight(g, sn, cx, cz, 0);
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
    c.g = g;
    c.sn = sn;
    c.doExpansionHack = jc->useExpansionHack;
    c.shapes = (JShape*) calloc(c.maxShapes, sizeof(JShape));
    c.qPiece = (int*) malloc(maxOut * sizeof(int));
    c.qShape = (int*) malloc(maxOut * sizeof(int));
    c.qTop = (int*) malloc(maxOut * sizeof(int));
    c.poolMax = (int*) malloc(jd->poolCount * sizeof(int));
    if (!c.shapes || !c.qPiece || !c.qShape || !c.qTop || !c.poolMax)
        goto done;
    memset(c.poolMax, 0xff, jd->poolCount * sizeof(int)); // all -1
    makeFeatureBlock(jd, &c.featureBlock);

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
    free(c.poolMax);
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
        uint64_t seed, int chunkX, int chunkZ, const Generator *g,
        const SurfaceNoise *sn, JigsawPiece *out, int maxOut)
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

    return getJigsawPieces(jd, &jc, startPool, &rng, chunkX * 16, chunkZ * 16,
            g, sn, out, maxOut);
}

int getJigsawPieceTemplates(const JigsawData *jd, const JigsawPiece *p,
        int16_t *out)
{
    const JigsawPoolEntryDef *e = &jd->poolEntries[p->entryIdx];
    int n = 0, i;

    if (e->kind == JIGSAW_ELEMENT_LIST) {
        for (i = 0; i < e->listCount && n < JIGSAW_MAX_PIECE_TEMPLATES; i++) {
            int t = elementTemplateIdx(jd, &jd->poolEntries[e->listStart + i]);
            if (t >= 0)
                out[n++] = (int16_t) t;
        }
        return n;
    }
    if (p->templateIdx >= 0)
        out[n++] = p->templateIdx;
    return n;
}

const char *getJigsawPieceFeature(const JigsawData *jd, const JigsawPiece *p)
{
    const JigsawPoolEntryDef *e = &jd->poolEntries[p->entryIdx];
    if (e->kind != JIGSAW_ELEMENT_FEATURE || e->featureIdx < 0)
        return NULL;
    return jd->strings[e->featureIdx];
}

int getJigsawFeaturePositions(const JigsawData *jd, const JigsawPiece *pieces,
        int nPieces, const char *feature, Pos3 *out, int maxOut)
{
    int n = 0, i;
    for (i = 0; i < nPieces; i++) {
        const char *f = getJigsawPieceFeature(jd, &pieces[i]);
        if (!f || strcmp(f, feature) != 0)
            continue;
        if (out) {
            if (n == maxOut)
                return n;
            out[n].x = pieces[i].x;
            out[n].y = pieces[i].y;
            out[n].z = pieces[i].z;
        }
        n++;
    }
    return n;
}

void getJigsawContainerPos(const JigsawData *jd, const JigsawPiece *p, int c, int *x, int *y, int *z)
{
    const JigsawTemplateDef *t = &jd->templates[p->templateIdx];
    const JigsawContainerDef *cd = &jd->containers[t->containerStart + c];
    transformPos(p->rotation, cd->x, cd->y, cd->z, x, y, z);
    *x += p->x; *y += p->y; *z += p->z;
}

int getJigsawLoot(const JigsawData *jd, StructureSaltConfig ssconf, int mc,
        uint64_t seed, const JigsawPiece *pieces, int nPieces,
        JigsawChest *out, int maxOut)
{
    int n = 0, i, ci, j;

    for (i = 0; i < nPieces; i++) {
        int16_t tmpl[JIGSAW_MAX_PIECE_TEMPLATES];
        int ntmpl = getJigsawPieceTemplates(jd, &pieces[i], tmpl), ti;
        for (ti = 0; ti < ntmpl; ti++) {
            const JigsawTemplateDef *t = &jd->templates[tmpl[ti]];
            for (ci = 0; ci < t->containerCount; ci++) {
                const JigsawContainerDef *cd = &jd->containers[t->containerStart + ci];
                JigsawChest *c;
                if (n == maxOut)
                    return -1;
                c = &out[n++];
                c->piece = i;
                transformPos(pieces[i].rotation, cd->x, cd->y, cd->z,
                             &c->x, &c->y, &c->z);
                c->x += pieces[i].x; c->y += pieces[i].y; c->z += pieces[i].z;
                c->lootSeed = 0;
                c->seedExact = 1;
                c->lootTable = cd->lootTable >= 0 ? jd->strings[cd->lootTable] : NULL;
            }
        }
    }
    
    for (i = 0; i < nPieces; i++) {
        int fcx, fcz;
        if (jd->poolEntries[pieces[i].entryIdx].kind != JIGSAW_ELEMENT_FEATURE)
            continue;
        fcx = pieces[i].x >> 4;
        fcz = pieces[i].z >> 4;
        for (j = 0; j < n; j++)
            if (out[j].piece > i && (out[j].x >> 4) == fcx && (out[j].z >> 4) == fcz)
                out[j].seedExact = 0;
    }

    for (i = 0; i < n; i++) {
        uint64_t r;
        int cx = out[i].x >> 4, cz = out[i].z >> 4;
        for (j = 0; j < i; j++)
            if ((out[j].x >> 4) == cx && (out[j].z >> 4) == cz)
                break;
        if (j < i) // an earlier chest already seeded this chunk
            continue;
        setSeed(&r, getPopulationSeed(mc, seed, cx * 16, cz * 16)
                    + ssconf.decoratorIndex + 10000 * ssconf.generationStep);
        for (j = i; j < n; j++)
            if ((out[j].x >> 4) == cx && (out[j].z >> 4) == cz)
                out[j].lootSeed = nextLong(&r);
    }
    return n;
}

int getJigsawStructureLoot(int structureType, int mc, int biome, uint64_t seed,
        int chunkX, int chunkZ, const Generator *g, const SurfaceNoise *sn,
        JigsawPiece *pieces, int maxPieces,
        int *nPieces, JigsawChest *chests, int maxChests)
{
    StructureSaltConfig ssconf;
    int n;

    n = getJigsawStructurePieces(structureType, mc, biome, seed,
            chunkX, chunkZ, g, sn, pieces, maxPieces);
    if (nPieces)
        *nPieces = n;
    if (n < 0)
        return -1;

    if (!getStructureSaltConfig(structureType, mc, biome, &ssconf))
        return -1;

    return getJigsawLoot(getJigsawData(mc), ssconf, mc, seed,
            pieces, n, chests, maxChests);
}
