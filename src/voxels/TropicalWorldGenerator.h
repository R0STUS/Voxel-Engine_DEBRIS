#ifndef VOXELS_TROPICALWORLDGENERATOR_H_
#define VOXELS_TROPICALWORLDGENERATOR_H_

#include "../typedefs.h"
#include "../voxels/WorldGenerator.h"

struct voxel;
class Content;

class TropicalWorldGenerator : WorldGenerator {
public:

	TropicalWorldGenerator(const Content* content) : WorldGenerator(content) {}

	void generate(voxel* voxels, int x, int z, int seed);
};

#endif /* VOXELS_TROPICALWORLDGENERATOR_H_ */