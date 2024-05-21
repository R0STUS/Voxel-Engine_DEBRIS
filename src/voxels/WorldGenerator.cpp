#include "WorldGenerator.h"
#include "voxel.h"
#include "Chunk.h"
#include "Block.h"

#include "../content/Content.h"

WorldGenerator::WorldGenerator(const Content* content)
               : idStone(content->requireBlock("base:stone").rt.id),
                 idDirt(content->requireBlock("base:dirt").rt.id),
                 idGrassBlock(content->requireBlock("base:grass_block").rt.id),
                 idSand(content->requireBlock("base:sand").rt.id),
                 idWater(content->requireBlock("base:water").rt.id),
                 idWood(content->requireBlock("base:wood").rt.id),
                 idLeaves(content->requireBlock("base:leaves").rt.id),
                 idGrass(content->requireBlock("base:grass").rt.id),
                 idFlower(content->requireBlock("base:flower").rt.id),
                 idBazalt(content->requireBlock("base:bazalt").rt.id),
                 idDebris(content->requireBlock("base:debris").rt.id),
                 idMoss(content->requireBlock("base:moss").rt.id),
                 idBrick(content->requireBlock("base:brick").rt.id),
                 idBrickDebris(content->requireBlock("base:brick_debris").rt.id),
                 idAir(content->requireBlock("core:air").rt.id),
                 idRust(content->requireBlock("base:rust").rt.id),
                 idJungleTrunk(content->requireBlock("base:rust").rt.id),
                 idJungleLeaves(content->requireBlock("base:rust").rt.id),
                 idVines(content->requireBlock("base:rust").rt.id), 
                 idPalmTrunk(content->requireBlock("base:rust").rt.id), 
                 idPalmLeaves(content->requireBlock("base:rust").rt.id) {}