#include "CubicWorldGenerator.h"
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

#include "../content/Content.h"
#include "../maths/voxmaths.h"
#include "../maths/util.h"
#include "../core_defs.h"

void CubicWorldGenerator::generate(voxel* voxels, int cx, int cz, int seed) {

    int padding = 8;

    float height = rand() % 59 + 1;

    float height2 = height;

    bool go = false;

    int SEA_LEVEL = 35;

    for (int z = -padding; z < CHUNK_D + padding; z++) {
        for (int x = -padding; x < CHUNK_W + padding; x++) {
            int cur_x = x + cx * CHUNK_W;
            int cur_z = z + cz * CHUNK_D;
            float height = rand() % 69 + 1;
            float w = pow(fmax(-abs(height - SEA_LEVEL) + 4, 0) / 6, 2);
            float h1 = -abs(height - SEA_LEVEL - 0.03);
            float h2 = abs(height - SEA_LEVEL + 0.04);
            float h = (h1 + h2) * 100;
            height += (h * w);
        }
    }

    for (int z = 0; z < CHUNK_D; z++) {
        int cur_z = z + cz * CHUNK_D;
        for (int x = 0; x < CHUNK_W; x++) {
            int cur_x = x + cx * CHUNK_W;
            while (!go) {
                float height = rand() % 49 + 1;
                if (height < 45) {
                }
                else
                    go = true;
            }
            float height2 = height;
            go = false;
            for (int cur_y = 0; cur_y < CHUNK_H; cur_y++) {
                int id = cur_y < SEA_LEVEL ? idWater : BLOCK_AIR;
                int states = 0;
                if (cur_y < (height - 6)) {
                    id = idStone;
                }
                else if (cur_y < height) {
                    id = idDirt;
                }
                if (cur_y == height && cur_y > SEA_LEVEL - 1) {
                    id = idGrassBlock;
                }
                if (cur_y <= 2)
                    id = idBazalt;
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].id = id;
                voxels[(cur_y * CHUNK_D + z) * CHUNK_W + x].states = states;
            }
        }
    }
}