#ifndef VOXELS_OCEANWORLDGENERATOR_H_
#define VOXELS_OCEANWORLDGENERATOR_H_

#include "../typedefs.h"
#include "../voxels/WorldGenerator.h"

struct voxel;
class Content;

class OceanWorldGenerator : WorldGenerator {
public:

	OceanWorldGenerator(const Content* content) : WorldGenerator(content) {}

	void generate(voxel* voxels, int x, int z, int seed);
};

#endif /* VOXELS_OCEANWORLDGENERATOR_H_ */