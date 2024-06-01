#include "SpaceWorldGenerator.h"
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

class perlin {
private:
    static const int permutation2[];
    static int p2[512];

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
            p2[i] = permutation2[i];
        }

        for (int i = 256; i < 512; i++) {
            p2[i] = p2[i - 256];
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

        int A = p2[X] + Y;
        int AA = p2[A] + Z;
        int AB = p2[A + 1] + Z;
        int B = p2[X + 1] + Y;
        int BA = p2[B] + Z;
        int BB = p2[B + 1] + Z;

        return lerp(w, lerp(v, lerp(u, grad(p2[AA], x, y, z),
            grad(p2[BA], x - 1, y, z)),
            lerp(u, grad(p2[AB], x, y - 1, z),
                grad(p2[BB], x - 1, y - 1, z))),
            lerp(v, lerp(u, grad(p2[AA + 1], x, y, z - 1),
                grad(p2[BA + 1], x - 1, y, z - 1)),
                lerp(u, grad(p2[AB + 1], x, y - 1, z - 1),
                    grad(p2[BB + 1], x - 1, y - 1, z - 1))));
    }
};

const int perlin::permutation2[] = { 151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197,
    62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
    87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27,
    166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105,
    92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
    73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188,
    159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16,
    58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154,
    163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251,
    34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235,
    249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
    176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114,
    67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };
int perlin::p2[512];

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

float calc_height3(perlin& noise, int cur_x, int cur_z) {
    float height = 0;

    height += noise.noise((float)cur_x * 0.0125f * 8 - 125567, 0, (float)cur_z * 0.0125f * 8 + 3546);
    height += noise.noise((float)cur_x * 0.025f * 8 + 4647, 0, (float)cur_z * 0.025f * 8 - 3436) * 0.5f;
    height += noise.noise((float)cur_x * 0.05f * 8 - 834176, 0, (float)cur_z * 0.05f * 8 + 23678) * 0.25f;
    height += noise.noise((float)cur_x * 0.2f * 8 + noise.noise((float)cur_x * 0.1f * 8 - 23557, 0, (float)cur_z * 0.1f * 8 - 6568) * 50,
        0, (float)cur_z * 0.2f * 8 + noise.noise((float)cur_x * 0.1f * 8 + 4363, 0, (float)cur_z * 0.1f * 8 + 4456) * 50) * noise.noise((float)cur_x * 0.01f - 834176, 0, (float)cur_z * 0.01f + 23678) * 0.25;
    height += noise.noise((float)cur_x * 0.1f * 8 - 3465, 0, (float)cur_z * 0.1f * 8 + 4534) * 0.125f;
    height *= noise.noise((float)cur_x * 0.1f + 1000, 0, (float)cur_z * 0.1f + 1000) * 0.5f + 0.5f;
    height += 1.0f;
    height *= 64.0f;
    return height;
}



void SpaceWorldGenerator::generate(voxel* voxels, int cx, int cz, int seed) {
    perlin::initPerlin();

    int padding = 8;
    Map2D heights(cx * CHUNK_W - padding,
        cz * CHUNK_D - padding,
        CHUNK_W + padding * 2,
        CHUNK_D + padding * 2);

    for (int z = -padding; z < CHUNK_D + padding; z++) {
        for (int x = -padding; x < CHUNK_W + padding; x++) {
            int cur_x = x + cx * CHUNK_W;
            int cur_z = z + cz * CHUNK_D;
            float height = perlin::noise((float)cur_x * 0.0125f * 8 - 125567, 0, (float)cur_z * 0.0125f * 8 + 3546);
            height += perlin::noise((float)cur_x * 0.025f * 8 + 4647, 0, (float)cur_z * 0.025f * 8 - 3436) * 0.5f;
            height += perlin::noise((float)cur_x * 0.05f * 8 - 834176, 0, (float)cur_z * 0.05f * 8 + 23678) * 0.25f;
            height += perlin::noise((float)cur_x * 0.2f * 8 + perlin::noise((float)cur_x * 0.1f * 8 - 23557, 0, (float)cur_z * 0.1f * 8 - 6568) * 50,
                0, (float)cur_z * 0.2f * 8 + perlin::noise((float)cur_x * 0.1f * 8 + 4363, 0, (float)cur_z * 0.1f * 8 + 4456) * 50) * perlin::noise((float)cur_x * 0.01f - 834176, 0, (float)cur_z * 0.01f + 23678) * 0.25;
            height += perlin::noise((float)cur_x * 0.1f * 8 - 3465, 0, (float)cur_z * 0.1f * 8 + 4534) * 0.125f;
            height *= perlin::noise((float)cur_x * 0.1f + 1000, 0, (float)cur_z * 0.1f + 1000) * 0.5f + 0.5f;
            height += 1.0f;
            height *= 64.0f;

            float hum = perlin::noise((float)cur_x * 0.3f + 633, 0, (float)cur_z * 0.3f);
            float sand = perlin::noise((float)cur_x * 0.1f - 633, 0, (float)cur_z * 0.1f + 1000);
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
            float height = heights.get(MAPS::HEIGHT, cur_x, cur_z) - 7;
            float height2 = height - SEA_LEVEL;

            for (int cur_y = 0; cur_y < CHUNK_H; cur_y++) {
                int id;
                int states = 0;

                if (cur_y < SEA_LEVEL) {
                    id = idAir;
                    int mirrored_y = SEA_LEVEL - cur_y;
                    if (mirrored_y < height2) {
                        id = idDebris;
                    }
                    else if (mirrored_y == (int)height) {
                        rnd = (std::rand() % 8) + 1;
                        if (rnd == 2) {
                            id = idAir;
                        }
                        else {
                            id = idAir;
                        }
                    }
                    else if (mirrored_y == (height + 1)) {
                        rnd = (std::rand() % 2) + 1;
                        tocalc++;
                        if (rnd == 2) {
                            totalgrass++;
                            id = idAir;
                        }
                        else {
                            id = idAir;
                        }
                        if (tocalc == 1000) {
                            tocalc = 0;
                            std::cout << "total grass: " << totalgrass << std::endl;
                        }
                    }
                    else if ((mirrored_y < (height - 6)) && (mirrored_y > (height - 30))) {
                        id = idAir;
                    }
                    else if (mirrored_y < height) {
                        id = idAir;
                    }
                }
                else {
                    id = cur_y < SEA_LEVEL ? idWater : BLOCK_AIR;
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
                }
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].id = id;
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].states = states;
            }
        }
    }
}
