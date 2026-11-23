#ifndef NOISE_H_
#define NOISE_H_

#include "rng.h"
#include <math.h>

STRUCT(PerlinNoise)
{
    uint8_t d[256+1];
    uint8_t h2;
    double a, b, c;
    double amplitude;
    double lacunarity;
    double d2;
    double t2;
};

STRUCT(OctaveNoise)
{
    int octcnt;
    PerlinNoise *octaves;
};

STRUCT(DoublePerlinNoise)
{
    double amplitude;
    OctaveNoise octA;
    OctaveNoise octB;
};

#ifdef __cplusplus
extern "C"
{
#endif

/// Helper
/// This is a highly performance critical function that is used to correct
/// progressing errors from float-maths. However, since cubiomes uses
/// doubles anyway, this seems useless in practice.
static inline ATTR(hot, const)
double maintainPrecision(double x)
{
    //return x - round(x / 33554432.0) * 33554432.0;
    return x;
}

/// Perlin noise
void perlinInit(PerlinNoise *__restrict noise, uint64_t *__restrict seed);
void xPerlinInit(PerlinNoise *__restrict noise, Xoroshiro *__restrict xr);

double samplePerlin(const PerlinNoise *noise, double x, double y, double z,
        double yamp, double ymax);
double sampleSimplex2D(const PerlinNoise *noise, double x, double y);

/// Initializes legacy octave noise using Java Random.
/// \param[out] noise output object
/// \param[in,out] seed Java Random state
/// \param omin first octave
/// \param len octave count, less than 64
void octaveInit(OctaveNoise *noise, uint64_t *seed, PerlinNoise *octaves,
        int omin, int len);
/// Initializes legacy octave noise using Xoroshiro.
/// \param[out] noise output object
/// \param[in,out] xr Xoroshiro state
/// \param omin first octave
/// \param len octave count, less than 64
void xOctaveLegacyInit(OctaveNoise *noise, Xoroshiro *xr, PerlinNoise *octaves,
        int omin, int len);
/// Initializes Beta octave noise with explicit frequency/amplitude progression.
/// \param[out] noise output object
/// \param[in,out] seed Java Random state
/// \param[out] octaves PerlinNoise buffer of size `octcnt`
/// \param octcnt number of octaves
/// \param lac first octave's frequency multiplier (lacunarity)
/// \param lacMul frequency multiplier applied for each successive octave
/// \param persist first octave's amplitude
/// \param persistMul amplitude multiplier applied for each successive octave
void octaveInitBeta(OctaveNoise *noise, uint64_t *seed, PerlinNoise *octaves,
        int octcnt, double lac, double lacMul, double persist, double persistMul);
/// Initializes MC 1.18+ octave noise.
/// \param[out] noise output object
/// \param[in,out] xr Xoroshiro state
/// \param[out] octaves PerlinNoise buffer with space for each initialized octave,
/// `len` elements always suffice
/// \param[in] amplitudes array of `len` octave amplitudes
/// \param omin first octave
/// \param len amplitude count
/// \param nmax maximum number of nonzero octaves to initialize, -1 for all
/// \return number of initialized elements in octaves (also noise->octcnt)
int xOctaveInit(OctaveNoise *noise, Xoroshiro *xr, PerlinNoise *octaves,
        const double *amplitudes, int omin, int len, int nmax);

double sampleOctave(const OctaveNoise *noise, double x, double y, double z);
double sampleOctaveAmp(const OctaveNoise *noise, double x, double y, double z,
        double yamp, double ymin, int ydefault);
double sampleOctave2D(const OctaveNoise *noise, double x, double z);
double sampleOctaveBeta17Biome(const OctaveNoise *noise, double x, double z);
void sampleOctaveBeta17Terrain(const OctaveNoise *noise, double *v,
        double x, double z, int yLacFlag, double lacmin);

/// Initializes two legacy octave samplers for double Perlin noise.
/// \param[out] noise output object
/// \param[in,out] seed Java Random state
/// \param[out] octavesA,octavesB distinct buffers of at least `len` PerlinNoise elements each
/// \param omin first octave
/// \param len octave count, less than 64
void doublePerlinInit(DoublePerlinNoise *noise, uint64_t *seed,
        PerlinNoise *octavesA, PerlinNoise *octavesB, int omin, int len);
/// Initializes a 1.18+ DoublePerlinNoise generator.
/// \param[out] noise output object
/// \param[in,out] xr Xoroshiro state
/// \param[out] octaves octaves buffer, size has to be 2 * (number nonzero entries in amplitudes)
/// \param[in] amplitudes octave amplitude, needs at least one nonzero entry
/// \param omin first octave
/// \param len amplitude count
/// \param nmax number of octaves available in buffer (can be <=0 to ignore)
/// \return number of octaves used (see octaves buffer size)
int xDoublePerlinInit(DoublePerlinNoise *noise, Xoroshiro *xr,
        PerlinNoise *octaves, const double *amplitudes, int omin, int len, int nmax);

double sampleDoublePerlin(const DoublePerlinNoise *noise,
        double x, double y, double z);


#ifdef __cplusplus
}
#endif

#endif /* NOISE_H_ */
