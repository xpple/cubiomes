#include <time.h>
#include <stdio.h>
#include <string.h>

#include "rng.h"
#include "xrms.h"

#define MAT_SIZE sizeof(uint64_t) * 128 * 2

static void calcMatPows(uint64_t m[128][2]);
static void transpose(uint64_t m[128][2], uint64_t t[128][2]);
static void calcMatMul(uint64_t m1[128][2], uint64_t m2[128][2], uint64_t m[128][2]);
static void printMatsToFile(uint64_t m[POWS_OF_2][128][2]);

static void calcMats() {
    // calculate where the basis vectors go
    // vectors are of the form [u64, u64] = [hi, lo]
    // m is then of the form
    // [ [u  [u  [u      [u  [u  [u
    //    6   6   6       6   6   6
    //    4   4   4       4   4   4
    //    ,   ,   ,       ,   ,   ,
    //    u   u   u       u   u   u
    //    6   6   6       6   6   6
    //    4], 4], 4], ... 4], 4], 4], ]
    uint64_t m[128][2];
    for (int c = 0; c < 128; c++) {
        uint64_t bv[2];
        if (c < 64) {
            bv[0] = 1ULL << (64 - c - 1);
            bv[1] = 0;
        } else {
            bv[0] = 0;
            bv[1] = 1ULL << (128 - c - 1);
        }
        Xoroshiro xr = {bv[1], bv[0]};
        xNextLong(&xr);
        m[c][0] = xr.hi;
        m[c][1] = xr.lo;
    }

    calcMatPows(m);
}

static void calcMatPows(uint64_t m[128][2]) {
    uint64_t pows[POWS_OF_2][128][2];
    // for matrix vector multiplication, we want m of the form
    // [ [u64, u64],
    //   [u64, u64],
    //   [u64, u64],
    //      ...
    //   [u64, u64],
    //   [u64, u64],
    //   [u64, u64], ]
    // to do wordwise operations (as opposed to bitwise):
    // [ [u64, u64],   [ u   [ p(u   u)   p(u   u)
    //   [u64, u64],     6     a(6 & 6) ^ a(6 & 6)
    //   [u64, u64],     4     r(4   4)   r(4   4)
    //      ...       *  ,  =           ,
    //   [u64, u64],     u     p(u   u)   p(u   u)
    //   [u64, u64],     6     a(6 & 6) ^ a(6 & 6)
    //   [u64, u64], ]   4 ]   r(4   4)   r(4   4) ]
    // as such, transpose the matrices

    uint64_t tmp[128][2] = {0};
    uint64_t tmp2[128][2] = {0};
    transpose(m, pows[0]);

    for (int pow = 1; pow < POWS_OF_2; ++pow) {
        transpose(pows[pow - 1], tmp);
        calcMatMul(pows[pow - 1], tmp, tmp2);
        transpose(tmp2, pows[pow]);
    }

    printMatsToFile(pows);
}

static void transpose(uint64_t m[128][2], uint64_t t[128][2]) {
    memset(t, 0, sizeof(uint64_t) * 128 * 2);
    for (int r = 0; r < 128; ++r) {
        for (int c = 0; c < 128; ++c) {
            t[r][c < 64 ? 0 : 1] |= ((m[c][r < 64 ? 0 : 1] >> (r < 64 ? 64 - r - 1 : 128 - r - 1)) & 1) << (c < 64 ? 64 - c - 1 : 128 - c - 1);
        }
    }
}

static void printMatsToFile(uint64_t m[POWS_OF_2][128][2]) {
    FILE* fp = fopen("xrms.h", "w");
    fprintf(fp, "#ifndef XRMS_H_\n#define XRMS_H_\n\n#include <stdint.h>\n\n#define POWS_OF_2 64\n\n");
    fprintf(fp, "const uint64_t ms[POWS_OF_2][128][2] = {\n");
    for (size_t i = 0; i < POWS_OF_2; ++i) {
        fprintf(fp, "    {\n");
        for (size_t j = 0; j < 128; ++j) {
            fprintf(fp, "        {%" PRIu64 "ULL, %" PRIu64 "ULL},\n", m[i][j][0], m[i][j][1]);
        }
        fprintf(fp, "    },\n");
    }
    fprintf(fp, "};\n\n#endif //XRMS_H_\n");
}

static void calcMatMul(uint64_t m1[128][2], uint64_t m2[128][2], uint64_t t[128][2]) {
    for (int c = 0; c < 128; c++) {
        Xoroshiro xr = {m2[c][1], m2[c][0]};
        calcVecMul(m1, &xr);
        t[c][0] = xr.hi;
        t[c][1] = xr.lo;
    }
}
