#include "fortress.h"

#include <string.h>

STRUCT(FortressPieceEnv)
{
    Piece *list;
    int *n;
    uint64_t *rng;
    int typlast;
    int nmax;
    int ntyp[FORTRESS_PIECE_COUNT];
};

static const struct
{
    Pos3 offset, size;
    int repeatable, weight, max;
    const char *name;
}
fortress_info[] = {
    {{ 0, 0,0}, {18, 9,18}, 0, 0, 0, "NeStart"}, // FORTRESS_START
    {{-1,-3,0}, { 4, 9,18}, 1,30, 0, "NeBS"},    // BRIDGE_STRAIGHT
    {{-8,-3,0}, {18, 9,18}, 0,10, 4, "NeBCr"},   // BRIDGE_CROSSING
    {{-2, 0,0}, { 6, 8, 6}, 0,10, 4, "NeRC"},    // BRIDGE_FORTIFIED_CROSSING
    {{-2, 0,0}, { 6,10, 6}, 0,10, 3, "NeSR"},    // BRIDGE_STAIRS
    {{-2, 0,0}, { 6, 7, 8}, 0, 5, 2, "NeMT"},    // BRIDGE_SPAWNER
    {{-5,-3,0}, {12,13,12}, 0, 5, 1, "NeCE"},    // BRIDGE_CORRIDOR_ENTRANCE
    {{-1, 0,0}, { 4, 6, 4}, 1,25, 0, "NeSC"},    // CORRIDOR_STRAIGHT
    {{-1, 0,0}, { 4, 6, 4}, 0,15, 5, "NeSCSC"},  // CORRIDOR_CROSSING
    {{-1, 0,0}, { 4, 6, 4}, 0, 5,10, "NeSCRT"},  // CORRIDOR_TURN_RIGHT
    {{-1, 0,0}, { 4, 6, 4}, 0, 5,10, "NeSCLT"},  // CORRIDOR_TURN_LEFT
    {{-1,-7,0}, { 4,13, 9}, 1,10, 3, "NeCCS"},   // CORRIDOR_STAIRS
    {{-3, 0,0}, { 8, 6, 8}, 0, 7, 2, "NeCTB"},   // CORRIDOR_T_CROSSING
    {{-5,-3,0}, {12,13,12}, 0, 5, 2, "NeCSR"},   // CORRIDOR_NETHER_WART
    {{-1,-3,0}, { 4, 9, 7}, 0, 0, 0, "NeBEF"},   // FORTRESS_END
};

static
Piece *addFortressPiece(FortressPieceEnv *env, int typ, int x, int y, int z, int depth, int facing, int pending)
{
    Pos3 pos = {x, y, z};
    Pos3 b0 = pos, b1 = pos;
    Pos3 d0 = fortress_info[typ].offset, d1 = fortress_info[typ].size;
    b0.y += d0.y;
    b1.y += d0.y+d1.y;
    switch (facing)
    {
    case 0: // 0, north
        b0.x += d0.x;       b0.z += d0.z-d1.z;
        b1.x += d0.x+d1.x;  b1.z += d0.z;
        break;
    case 1: // 90, east
        b0.x += d0.z;       b0.z += d0.x;
        b1.x += d0.z+d1.z;  b1.z += d0.x+d1.x;
        break;
    case 2: // 180, south
        b0.x += d0.x;       b0.z += d0.z;
        b1.x += d0.x+d1.x;  b1.z += d0.z+d1.z;
        break;
    case 3: // 270, west
        b0.x += d0.z-d1.z;  b0.z += d0.x;
        b1.x += d0.z;       b1.z += d0.x+d1.x;
        break;
    }
    Piece *p = env->list + *env->n;
    p->name = fortress_info[typ].name;
    p->pos = pos;
    p->bb0 = b0;
    p->bb1 = b1;
    p->rot = facing;
    p->depth = depth;
    p->type = typ;
    p->next = NULL;

    int i, n = *env->n;
    for (i = 0; i < n; i++)
    {
        Piece *q = env->list + i;
        if (hasIntersection(q->bb0, q->bb1, p->bb0, p->bb1)) {
            return NULL; // collision
        }
    }
    if (typ == CORRIDOR_TURN_LEFT || typ == CORRIDOR_TURN_RIGHT) {
        p->chestCount = nextInt(env->rng, 3) == 0;
    } else if (typ == FORTRESS_END) {
        skipNextN(env->rng, 1);
    }
    // accept the piece and append it to the processing front
    //int queue = 0;
    if (pending)
    {
        (*env->n)++;
        env->ntyp[typ]++;
        if (typ != FORTRESS_END)
            env->typlast = typ;
        Piece *q = env->list;
        while (q->next) {
            q = q->next; //queue++;
        }
        q->next = p;
    }
    //printf("[%3d] typ=%2d @(%4d %4d %4d) f=%d p=%d queue=%2d   rng:%ld\n",
    //    (*env->n-1), typ, b0.x, b0.y, b0.z, facing, pending, queue, *env->rng);
    //fflush(stdout);
    return p;
}


static
void extendFortress(FortressPieceEnv *env, Piece *p, int offh, int offv, int turn, int corridor)
{
    int x, y, z, t, i;
    int depth = p->depth + 1;
    int facing = p->rot;
    int typ0 = corridor ? CORRIDOR_STRAIGHT : BRIDGE_STRAIGHT;
    int typ1 = typ0 + (corridor ? 7 : 6);
    int valid = -1;
    int weight_tot = 0;

    y = p->bb0.y + offv;

    if (turn == 0) { // forward
        switch (facing) {
        case 0: x = p->bb0.x+offh; z = p->bb0.z-1;    break;
        case 1: x = p->bb1.x+1;    z = p->bb0.z+offh; break;
        case 2: x = p->bb0.x+offh; z = p->bb1.z+1;    break;
        case 3: x = p->bb0.x-1;    z = p->bb0.z+offh; break;
        default: UNREACHABLE();
        }
    } else if (turn == -1) { // left
        if (facing & 1) { x = p->bb0.x+offh; z = p->bb0.z-1;    facing = 0; }
        else            { x = p->bb0.x-1;    z = p->bb0.z+offh; facing = 3; }
    } else if (turn == +1) { // right
        if (facing & 1) { x = p->bb0.x+offh, z = p->bb1.z+1;    facing = 2; }
        else            { x = p->bb1.x+1;    z = p->bb0.z+offh; facing = 1; }
    } else UNREACHABLE();

    if (IABS(x - env->list->bb0.x) > 112 || IABS(z - env->list->bb0.z) > 112)
        goto L_end;

    for (valid = 0, t = typ0; t < typ1; t++)
    {
        int max = fortress_info[t].max;
        if (max > 0 && env->ntyp[t] >= max)
            continue;
        if (max > 0)
            valid = 1;
        weight_tot += fortress_info[t].weight;
    }

    if (valid == 0 || weight_tot <= 0 || depth > 30)
        goto L_end;

    for (i = 0; i < 5; i++)
    {
        int n = nextInt(env->rng, weight_tot);
        for (t = typ0; t < typ1; t++)
        {
            int max = fortress_info[t].max;
            if (max > 0 && env->ntyp[t] >= max)
                continue;
            n -= fortress_info[t].weight;
            if (n >= 0)
                continue;
            if (env->typlast == t && !fortress_info[t].repeatable)
                break;
            if (addFortressPiece(env, t, x, y, z, depth, facing, 1) != NULL)
                return;
        }
    }

L_end:
    addFortressPiece(env, FORTRESS_END, x, y, z, depth, facing, valid >= 0);
}

static
void extendFortressPiece(FortressPieceEnv *env, Piece *p)
{
    if (p->type == BRIDGE_STRAIGHT) {
        extendFortress(env, p, 1, 3,  0, 0);
    } else if (p->type == BRIDGE_CROSSING || p->type == FORTRESS_START) {
        extendFortress(env, p, 8, 3,  0, 0);
        extendFortress(env, p, 8, 3, -1, 0);
        extendFortress(env, p, 8, 3,  1, 0);
    } else if (p->type == BRIDGE_FORTIFIED_CROSSING) {
        extendFortress(env, p, 2, 0,  0, 0);
        extendFortress(env, p, 2, 0, -1, 0);
        extendFortress(env, p, 2, 0,  1, 0);
    } else if (p->type == BRIDGE_STAIRS) {
        extendFortress(env, p, 2, 6,  1, 0);
    } else if (p->type == BRIDGE_CORRIDOR_ENTRANCE) {
        extendFortress(env, p, 5, 3,  0, 1);
    } else if (p->type == CORRIDOR_STRAIGHT) {
        extendFortress(env, p, 1, 0,  0, 1);
    } else if (p->type == CORRIDOR_CROSSING) {
        extendFortress(env, p, 1, 0,  0, 1);
        extendFortress(env, p, 1, 0, -1, 1);
        extendFortress(env, p, 1, 0,  1, 1);
    } else if (p->type == CORRIDOR_TURN_RIGHT) {
        extendFortress(env, p, 1, 0,  1, 1);
    } else if (p->type == CORRIDOR_TURN_LEFT) {
        extendFortress(env, p, 1, 0, -1, 1);
    } else if (p->type == CORRIDOR_STAIRS) {
        extendFortress(env, p, 1, 0,  0, 1);
    } else if (p->type == CORRIDOR_T_CROSSING) {
        int h = (p->rot == 0 || p->rot == 3) ? 5 : 1;
        extendFortress(env, p, h, 0, -1, nextInt(env->rng, 8) != 0);
        extendFortress(env, p, h, 0,  1, nextInt(env->rng, 8) != 0);
    } else if (p->type == CORRIDOR_NETHER_WART) {
        extendFortress(env, p, 5, 3,  0, 1);
        extendFortress(env, p, 5, 11, 0, 1);
    }
}

int getFortressPieces(Piece *list, int n, int mc, uint64_t seed, int chunkX, int chunkZ)
{
    uint64_t rng = seed;
    if (mc <= MC_1_15)
    {
        setAttemptSeed(&rng, chunkX, chunkZ);
        nextInt(&rng, 3);
        nextInt(&rng, 8);
        nextInt(&rng, 8);
    }
    else
    {
        rng = chunkGenerateRnd(seed, chunkX, chunkZ);
    }

    int count = 1;
    FortressPieceEnv env;
    memset(&env, 0, sizeof(env));
    env.list = list;
    env.n = &count;
    env.rng = &rng;
    env.ntyp[0] = 1;
    env.typlast = 0;
    env.nmax = n;
    Piece *p = list;
    Pos3 pos = {chunkX * 16 + 2, 64, chunkZ * 16 + 2};
    p->name = fortress_info[0].name;
    p->bb0 = p->bb1 = p->pos = pos;
    p->bb1.x += fortress_info[0].size.x;
    p->bb1.y += fortress_info[0].size.y;
    p->bb1.z += fortress_info[0].size.z;
    if (mc <= MC_1_7_10) {
        switch(nextInt(&rng, 4)) {
        case 0: p->rot = 2; break;
        case 1: p->rot = 3; break;
        case 2: p->rot = 0; break;
        case 3: p->rot = 1; break;
        default: UNREACHABLE();
        }
    } else {
        p->rot = nextInt(&rng, 4);
    }
    p->depth = 0;
    p->type = 0;
    p->next = NULL;
    extendFortressPiece(&env, p);
    while (list->next)
    {
        Piece *q = list;
        int len = 0;
        while (q->next)
        {
            q = q->next;
            len++;
        }
        int i = nextInt(&rng, len);
        for (p = list, q = list->next; i-->0; p = q, q = q->next);
        p->next = q->next;
        q->next = NULL;
        extendFortressPiece(&env, q);
    }
    return count;
}
