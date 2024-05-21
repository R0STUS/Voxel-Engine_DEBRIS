#ifndef VOXELS_WORLDGENERATOR_H_
#define VOXELS_WORLDGENERATOR_H_

#include "../typedefs.h"
#include <string>

struct voxel;
class Content;

class WorldGenerator {
protected:
	blockid_t const idStone;
	blockid_t const idDirt;
	blockid_t const idGrassBlock;
	blockid_t const idSand;
	blockid_t const idWater;
	blockid_t const idWood;
	blockid_t const idLeaves;
	blockid_t const idGrass;
	blockid_t const idFlower;
	blockid_t const idBazalt;
	blockid_t const idDebris;
	blockid_t const idMoss;
	blockid_t const idBrick;
	blockid_t const idBrickDebris;
	blockid_t const idAir;
	blockid_t const idRust;
	blockid_t const idJungleTrunk;
	blockid_t const idJungleLeaves;
	blockid_t const idVines;
	blockid_t const idPalmTrunk;
	blockid_t const idPalmLeaves;
public:
	WorldGenerator(const Content* content);
    virtual ~WorldGenerator() = default;

	virtual void generate(voxel* voxels, int x, int z, int seed) = 0;
};

#endif /* VOXELS_WORLDGENERATOR_H_ */