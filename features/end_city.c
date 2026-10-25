#include "end_city.h"

#include <string.h>

STRUCT(EndCityPieceEnv)
{
    Piece *list;
    int *n;
    uint64_t *rng;
    int *ship;
    int y;
};

typedef int (piecefunc_t)(EndCityPieceEnv *env, Piece *current, int depth);

static piecefunc_t genTower;
static piecefunc_t genBridge;
static piecefunc_t genHouseTower;
static piecefunc_t genFatTower;

static
Piece *addEndCityPiece(EndCityPieceEnv *env, Piece *prev, int rot, int px, int py, int pz, int typ)
{
    static const struct { int sx, sy, sz; const char *name; } info[] = {
        {  9,  3,  9, "base_floor"},
        { 11,  1, 11, "base_roof"},
        {  4,  5,  1, "bridge_end"},
        {  4,  6,  7, "bridge_gentle_stairs"},
        {  4,  5,  3, "bridge_piece"},
        {  4,  6,  3, "bridge_steep_stairs"},
        { 12,  3, 12, "fat_tower_base"},
        { 12,  7, 12, "fat_tower_middle"},
        { 16,  5, 16, "fat_tower_top"},
        { 11,  7, 11, "second_floor_1"},
        { 11,  7, 11, "second_floor_2"},
        { 13,  1, 13, "second_roof"},
        { 12, 23, 28, "ship"},
        { 13,  7, 13, "third_floor_1"},
        { 13,  7, 13, "third_floor_2"},
        { 15,  1, 15, "third_roof"},
        {  6,  6,  6, "tower_base"},
        {  6,  3,  6, "tower_floor"}, // unused
        {  6,  3,  6, "tower_piece"},
        {  8,  4,  8, "tower_top"},
    };

    Piece *p = env->list + *env->n;
    (*env->n)++;
    p->name = info[typ].name;
    p->rot = rot;
    p->depth = 0;
    p->type = typ;
    p->next = NULL;

    Pos3 pos = {px, py, pz};
    if (prev)
        pos = prev->pos;
    p->bb0 = p->bb1 = p->pos = pos;
    p->bb1.y += info[typ].sy;
    switch (rot)
    {
    case 0: p->bb1.x += info[typ].sx; p->bb1.z += info[typ].sz; break; // 0
    case 1: p->bb0.x -= info[typ].sz; p->bb1.z += info[typ].sx; break; // 90
    case 2: p->bb0.x -= info[typ].sx; p->bb0.z -= info[typ].sz; break; // 180
    case 3: p->bb1.x += info[typ].sz; p->bb0.z -= info[typ].sx; break; // 270
    default: UNREACHABLE();
    }
    if (prev)
    {
        int dx = 0, dy = py, dz = 0;
        switch (prev->rot)
        {
        case 0: dx += px; dz += pz; break; // 0
        case 1: dx -= pz; dz += px; break; // 90
        case 2: dx -= px; dz -= pz; break; // 180
        case 3: dx += pz; dz -= px; break; // 270
        default: UNREACHABLE();
        }
        p->pos.x += dx; p->pos.y += dy; p->pos.z += dz;
        p->bb0.x += dx; p->bb0.y += dy; p->bb0.z += dz;
        p->bb1.x += dx; p->bb1.y += dy; p->bb1.z += dz;
    }
    return p;
}

static
int genPiecesRecusively(piecefunc_t gen, EndCityPieceEnv *env, Piece *current, int depth)
{
    if (depth > 8)
        return 0;
    int i, j, n_local = 0;
    EndCityPieceEnv env_local = *env;
    env_local.list = env->list + *env->n;
    env_local.n = &n_local;
    if (!gen(&env_local, current, depth))
        return 0;
    int gendepth = next(env->rng, 32);
    for (i = 0; i < n_local; i++)
    {
        Piece *p = env_local.list + i;
        p->depth = gendepth;
        for (j = 0; j < *env->n; j++)
        {   // check for piece with bounding box collition
            Piece *q = env->list + j;
            if (q->bb1.x >= p->bb0.x && q->bb0.x <= p->bb1.x &&
                q->bb1.z >= p->bb0.z && q->bb0.z <= p->bb1.z &&
                q->bb1.y >= p->bb0.y && q->bb0.y <= p->bb1.y)
            {
                if (current->depth != q->depth)
                    return 0;
                break;
            }
        }
    }
    (*env->n) += n_local;
    return 1;
}

static
int genTower(EndCityPieceEnv *env, Piece *current, int depth)
{
    int rot = current->rot;
    int x = 3 + nextInt(env->rng, 2);
    int z = 3 + nextInt(env->rng, 2);
    Piece *base = current;
    base = addEndCityPiece(env, base, rot, x, -3, z, TOWER_BASE);
    base = addEndCityPiece(env, base, rot, 0, 7, 0, TOWER_PIECE);
    Piece *floor = (nextInt(env->rng, 3) == 0 ? base : NULL);
    int floorcnt = 1 + nextInt(env->rng, 3);
    int i;
    for (i = 0; i < floorcnt; i++)
    {
        base = addEndCityPiece(env, base, rot, 0, 4, 0, TOWER_PIECE);
        if (i < floorcnt - 1 && next(env->rng, 1))
            floor = base;
    }
    if (floor)
    {
        static const int binfo[][4] = {
            {0, 1, -1, 0}, // 0
            {1, 6, -1, 1}, // 90
            {3, 0, -1, 5}, // 270
            {2, 5, -1, 6}, // 180
        };
        for (i = 0; i < 4; i++)
        {
            if (!next(env->rng, 1))
                continue;
            int brot = (rot + binfo[i][0]) & 3;
            Piece *bridge = addEndCityPiece(env, base, brot,
                binfo[i][1], binfo[i][2], binfo[i][3], BRIDGE_END);
            genPiecesRecusively(genBridge, env, bridge, depth+1);
        }
    }
    else if (depth != 7)
    {
        return genPiecesRecusively(genFatTower, env, base, depth+1);
    }

    addEndCityPiece(env, base, rot, -1, 4, -1, TOWER_TOP);
    return 1;
}

static
int genBridge(EndCityPieceEnv *env, Piece *current, int depth)
{
    int rot = current->rot;
    int i, y, floorcnt = 1 + nextInt(env->rng, 4);
    Piece *base = current;
    base = addEndCityPiece(env, base, rot, 0, 0, -4, BRIDGE_PIECE);
    base->depth = -1;
    for (i = y = 0; i < floorcnt; i++)
    {
        if (next(env->rng, 1))
        {
            base = addEndCityPiece(env, base, rot, 0, y, -4, BRIDGE_PIECE);
            y = 0;
            continue;
        }
        if (next(env->rng, 1))
            base = addEndCityPiece(env, base, rot, 0, y, -4, BRIDGE_STEEP_STAIRS);
        else
            base = addEndCityPiece(env, base, rot, 0, y, -8, BRIDGE_GENTLE_STAIRS);
        y = 4;
    }
    if (!*env->ship && nextInt(env->rng, 10 - depth) == 0)
    {
        int x = -8 + nextInt(env->rng, 8);
        int z = -70 + nextInt(env->rng, 10);
        base = addEndCityPiece(env, base, rot, x, y, z, END_SHIP);
        *env->ship = 1;
    }
    else
    {
        env->y = y + 1;
        if (!genPiecesRecusively(genHouseTower, env, base, depth+1))
            return 0;
    }
    base = addEndCityPiece(env, base, (rot+2)&3, 4, y, 0, BRIDGE_END);
    base->depth = -1;
    return 1;
}

static
int genHouseTower(EndCityPieceEnv *env, Piece *current, int depth)
{
    if (depth > 8) return 0;
    int rot = current->rot;
    Piece *base = current;
    base = addEndCityPiece(env, base, rot, -3, env->y, -11, BASE_FLOOR);
    int size = nextInt(env->rng, 3);
    if (size == 0)
    {
        addEndCityPiece(env, base, rot, -1, 4, -1, BASE_ROOF);
        return 1;
    }
    base = addEndCityPiece(env, base, rot, -1, 0, -1, SECOND_FLOOR_2);
    if (size == 1)
    {
        base = addEndCityPiece(env, base, rot, -1, 8, -1, SECOND_ROOF);
    }
    else
    {
        base = addEndCityPiece(env, base, rot, -1, 4, -1, THIRD_FLOOR_2);
        base = addEndCityPiece(env, base, rot, -1, 8, -1, THIRD_ROOF);
    }
    genPiecesRecusively(genTower, env, base, depth+1);
    return 1;
}

static
int genFatTower(EndCityPieceEnv *env, Piece *current, int depth)
{
    int rot = current->rot;
    int i, j;
    Piece *base = current;
    base = addEndCityPiece(env, base, rot, -3, 4, -3, FAT_TOWER_BASE);
    base = addEndCityPiece(env, base, rot, 0, 4, 0, FAT_TOWER_MIDDLE);
    static const int binfo[][4] = {
        {0,  4, -1,  0}, // 0
        {1, 12, -1,  4}, // 90
        {3,  0, -1,  8}, // 270
        {2,  8, -1, 12}, // 180
    };
    for (j = 0; j < 2 && nextInt(env->rng, 3) != 0; j++)
    {
        base = addEndCityPiece(env, base, rot, 0, 8, 0, FAT_TOWER_MIDDLE);
        for (i = 0; i < 4; i++)
        {
            if (!next(env->rng, 1))
                continue;
            int brot = (rot + binfo[i][0]) & 3;
            Piece *bridge = addEndCityPiece(env, base, brot,
                binfo[i][1], binfo[i][2], binfo[i][3], BRIDGE_END);
            genPiecesRecusively(genBridge, env, bridge, depth+1);
        }
    }
    addEndCityPiece(env, base, rot, -2, 8, -2, FAT_TOWER_TOP);
    return 1;
}

int getEndCityPieces(Piece *list, uint64_t seed, int chunkX, int chunkZ)
{
    uint64_t rng = chunkGenerateRnd(seed, chunkX, chunkZ);
    int rot = nextInt(&rng, 4);
    int ship = 0, n = 0;
    EndCityPieceEnv env;
    memset(&env, 0, sizeof(env));
    env.list = list;
    env.n = &n;
    env.rng = &rng;
    env.ship = &ship;
    Piece *base = NULL;
    int x = chunkX * 16 + 8, z = chunkZ * 16 + 8;
    base = addEndCityPiece(&env, base, rot, x, 0, z, BASE_FLOOR);
    base = addEndCityPiece(&env, base, rot, -1, 0, -1, SECOND_FLOOR_1);
    base = addEndCityPiece(&env, base, rot, -1, 4, -1, THIRD_FLOOR_1);
    base = addEndCityPiece(&env, base, rot, -1, 8, -1, THIRD_ROOF);
    genPiecesRecusively(genTower, &env, base, 1);
    return n;
}
