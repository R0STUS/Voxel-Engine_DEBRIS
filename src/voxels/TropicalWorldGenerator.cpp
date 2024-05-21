#include "TropicalWorldGenerator.h"
#include "voxel.h"
#include "Chunk.h"
#include <cstdlib>
#include "Block.h"

#include <iostream>
#include <vector>
#include <time.h>
#include <stdexcept>
#include <math.h>
#include <cmath>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include "../maths/FastNoiseLite.h"
#include <algorithm>

#include "../content/Content.h"
#include "../maths/voxmaths.h"
#include "../maths/util.h"
#include "../core_defs.h"

const int SEA_LEVEL = 75;

class perlin2 {
private:
    static const int permutation[];
    static int p[512];

    static float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    static float grad(int hash, float x, float y, float z) {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    static void initPerlin() {
        for (int i = 0; i < 256; i++) {
            p[i] = permutation[i];
        }

        for (int i = 256; i < 512; i++) {
            p[i] = p[i - 256];
        }
    }

    static float noise(float x, float y, float z) {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        int Z = (int)std::floor(z) & 255;

        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        float u = fade(x);
        float v = fade(y);
        float w = fade(z);

        int A = p[X] + Y;
        int AA = p[A] + Z;
        int AB = p[A + 1] + Z;
        int B = p[X + 1] + Y;
        int BA = p[B] + Z;
        int BB = p[B + 1] + Z;

        return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
            grad(p[BA], x - 1, y, z)),
            lerp(u, grad(p[AB], x, y - 1, z),
                grad(p[BB], x - 1, y - 1, z))),
            lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                grad(p[BA + 1], x - 1, y, z - 1)),
                lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                    grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }

    static float fbm(float x, float y, float z, int octaves, float persistence) {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; i++) {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0f;
        }

        return total / maxValue;
    }
};

const int perlin2::permutation[] = { 91, 97, 72, 91, 90, 76,
85, 74, 98, 95, 96, 79, 99, 100, 70, 100, 94, 73, 92, 71, 75, 95,
70, 99, 74, 100, 72, 71, 74, 98, 70, 96, 100, 89, 100, 75, 70, 73, 99,
78, 94, 100, 97, 99, 87, 75, 71, 75, 78, 96, 74, 88, 100, 96, 77,
87, 95, 72, 91, 93, 94, 95, 76, 95, 74, 92, 71, 92, 94, 76, 73,
93, 77, 95, 97, 100, 83, 85, 100, 89, 78, 99, 93, 100, 98, 86,
92, 75, 77, 76, 100, 75, 100, 90, 95, 77, 80, 73, 79, 97, 70, 98, 80,
73, 96, 76, 92, 98, 99, 89, 72, 94, 99, 99, 93, 91, 87, 98,
97, 86, 93, 100, 85, 99, 95, 98, 71, 80, 77, 98, 100, 100, 90, 89,
70, 99, 74, 96, 88, 91, 100, 82, 85, 99, 99, 99, 78, 100, 76, 72,
78, 72, 97, 98, 73, 75, 100, 97, 94, 99, 89, 100, 96, 70, 75, 96,
93, 70, 98, 96, 86, 96, 94, 75, 95, 71, 91, 73, 75, 100, 72, 99,
85, 85, 79, 86, 100, 100, 96, 98, 85, 87, 98, 100, 97, 100, 100,
75, 100, 99, 100, 99, 95, 73, 98, 96, 97, 100, 81, 77, 95, 100,
100, 73, 100, 85, 76, 99, 98, 72, 97, 99, 87, 97, 98, 84, 99,
96, 87, 89, 77, 75, 91, 70, 96, 100, 94, 100, 99, 93, 100, 87,
80, 73, 73, 72, 100, 95, 90, 99, 78, 80, 98, 79, 96, 97 };
int perlin2::p[512];

enum class MAPS {
    SAND,
    CLIFF,
    HEIGHT
};
#define MAPS_LEN 4

class Map2D {
    int x, z;
    int w, d;
    float* heights[MAPS_LEN];
public:
    Map2D(int x, int z, int w, int d) : x(x), z(z), w(w), d(d) {
        for (int i = 0; i < MAPS_LEN; i++)
            heights[i] = new float[w * d];
    }
    ~Map2D() {
        for (int i = 0; i < MAPS_LEN; i++)
            delete[] heights[i];
    }

    inline float get(MAPS map, int x, int z) {
        x -= this->x;
        z -= this->z;
        if (x < 0 || z < 0 || x >= w || z >= d) {
            throw std::runtime_error("out of heightmap");
        }
        return heights[(int)map][z * w + x];
    }

    inline void set(MAPS map, int x, int z, float value) {
        x -= this->x;
        z -= this->z;
        if (x < 0 || z < 0 || x >= w || z >= d) {
            throw std::runtime_error("out of heightmap");
        }
        heights[(int)map][z * w + x] = value;
    }
};

float calc_height3(perlin2& noise, int cur_x, int cur_z) {
    float height = 0;

    height += noise.fbm((float)cur_x * 0.0125f * 8 - 125567, 0, (float)cur_z * 0.0125f * 8 + 3546, 6, 0.5);
    height += noise.fbm((float)cur_x * 0.025f * 8 + 4647, 0, (float)cur_z * 0.025f * 8 - 3436, 6, 0.5) * 0.5f;
    height += noise.fbm((float)cur_x * 0.05f * 8 - 834176, 0, (float)cur_z * 0.05f * 8 + 23678, 6, 0.5) * 0.25f;
    height += noise.fbm((float)cur_x * 0.2f * 8 + noise.fbm((float)cur_x * 0.1f * 8 - 23557, 0, (float)cur_z * 0.1f * 8 - 6568, 6, 0.5) * 50,
        0, (float)cur_z * 0.2f * 8 + noise.fbm((float)cur_x * 0.1f * 8 + 4363, 0, (float)cur_z * 0.1f * 8 + 4456, 6, 0.5) * 50, 6, 0.5) * noise.fbm((float)cur_x * 0.01f - 834176, 0, (float)cur_z * 0.01f + 23678, 6, 0.5) * 0.25;
    height += noise.fbm((float)cur_x * 0.1f * 8 - 3465, 0, (float)cur_z * 0.1f * 8 + 4534, 6, 0.5) * 0.125f;
    height *= noise.fbm((float)cur_x * 0.1f + 1000, 0, (float)cur_z * 0.1f + 1000, 6, 0.5) * 0.5f + 0.5f;
    height += 1.0f;
    height *= 64.0f;
    return height;
}



void TropicalWorldGenerator::generate(voxel* voxels, int cx, int cz, int seed) {
    perlin2::initPerlin();

    int padding = 8;
    Map2D heights(cx * CHUNK_W - padding,
        cz * CHUNK_D - padding,
        CHUNK_W + padding * 2,
        CHUNK_D + padding * 2);

    for (int z = -padding; z < CHUNK_D + padding; z++) {
        for (int x = -padding; x < CHUNK_W + padding; x++) {
            int cur_x = x + cx * CHUNK_W;
            int cur_z = z + cz * CHUNK_D;
            float height = perlin2::fbm((float)cur_x * 0.0125f * 8 - 125567, 0, (float)cur_z * 0.0125f * 8 + 3546, 6, 0.5);
            height += perlin2::fbm((float)cur_x * 0.025f * 8 + 4647, 0, (float)cur_z * 0.025f * 8 - 3436, 6, 0.5) * 0.5f;
            height += perlin2::fbm((float)cur_x * 0.05f * 8 - 834176, 0, (float)cur_z * 0.05f * 8 + 23678, 6, 0.5) * 0.25f;
            height += perlin2::fbm((float)cur_x * 0.2f * 8 + perlin2::fbm((float)cur_x * 0.1f * 8 - 23557, 0, (float)cur_z * 0.1f * 8 - 6568, 6, 0.5) * 50,
                0, (float)cur_z * 0.2f * 8 + perlin2::fbm((float)cur_x * 0.1f * 8 + 4363, 0, (float)cur_z * 0.1f * 8 + 4456, 6, 0.5) * 50, 6, 0.5) * perlin2::fbm((float)cur_x * 0.01f - 834176, 0, (float)cur_z * 0.01f + 23678, 6, 0.5) * 0.25;
            height += perlin2::fbm((float)cur_x * 0.1f * 8 - 3465, 0, (float)cur_z * 0.1f * 8 + 4534, 6, 0.5) * 0.125f;
            height *= perlin2::fbm((float)cur_x * 0.1f + 1000, 0, (float)cur_z * 0.1f + 1000, 6, 0.5) * 0.5f + 0.5f;
            height += 1.0f;
            height *= 64.0f;

            float hum = perlin2::fbm((float)cur_x * 0.3f + 633, 0, (float)cur_z * 0.3f, 6, 0.5);
            float sand = perlin2::fbm((float)cur_x * 0.1f - 633, 0, (float)cur_z * 0.1f + 1000, 6, 0.5);
            float cliff = pow((sand + abs(sand)) / 2, 1);
            float w = pow(fmax(-abs(height - SEA_LEVEL) + 4, 0) / 6, 2) * cliff;
            float h1 = -abs(height - SEA_LEVEL - 0.03);
            float h2 = abs(height - SEA_LEVEL + 0.04);
            float h = (h1 + h2) * 100;

            if (sand > 0.7f) {
                h *= 2.0f;
            }

            height += (h * w - 2 * h * w + 2);
            heights.set(MAPS::HEIGHT, cur_x, cur_z, height);
            heights.set(MAPS::SAND, cur_x, cur_z, sand);
            heights.set(MAPS::CLIFF, cur_x, cur_z, cliff);
        }
    }

    int rnd = 0;
    int totalgrass = 0;
    int tocalc = 0;

    for (int z = 0; z < CHUNK_D; z++) {
        int cur_z = z + cz * CHUNK_D;
        for (int x = 0; x < CHUNK_W; x++) {
            int cur_x = x + cx * CHUNK_W;
            float height = heights.get(MAPS::HEIGHT, cur_x, cur_z);


            for (int cur_y = 0; cur_y < CHUNK_H; cur_y++) {
                int id = cur_y < SEA_LEVEL ? idWater : BLOCK_AIR;
                int states = 0;
                if ((cur_y == (int)height) && (SEA_LEVEL - 2 < cur_y)) {
                    rnd = (std::rand() % 8) + 1;
                    if (rnd == 2) {
                        id = idMoss;
                    }
                    else {
                        if (cur_y > SEA_LEVEL) {
                            id = idDebris;
                        }
                        else if (cur_y < SEA_LEVEL) {
                            id = idWater;
                        }
                    }
                }
                else if (cur_y == (height + 1) && cur_y > SEA_LEVEL) {
                    rnd = (std::rand() % 2) + 1;
                    tocalc++;
                    if (rnd == 2) {
                        totalgrass++;
                        id = idGrass;
                    }
                    else {
                        id = idAir;
                    }
                    if (tocalc == 1000) {
                        tocalc = 0;
                        std::cout << "total grass: " << totalgrass << std::endl;
                    }
                }
                else if ((cur_y < (height - 6)) && (cur_y > (height - 30))) {
                    id = idBrickDebris;
                }
                else if (cur_y < height) {
                    id = idDebris;
                }
                float sand = fmax(heights.get(MAPS::SAND, cur_x, cur_z), heights.get(MAPS::CLIFF, cur_x, cur_z));
                if (((height - (1.1 - 0.2 * pow(height - 54, 4)) +
                    (10 * sand)) < cur_y + (height - 0.01 - (int)height))
                    && (cur_y < height)) {
                    id = idSand;
                }
                if (cur_y <= 2)
                    id = idBazalt;
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].id = id;
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].states = states;
            }
        }
    }
}
