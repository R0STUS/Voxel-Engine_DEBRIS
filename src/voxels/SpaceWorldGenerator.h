#ifndef VOXELS_SPACEWORLDGENERATOR_H_
#define VOXELS_SPACEWORLDGENERATOR_H_

#include "../typedefs.h"
#include "../voxels/WorldGenerator.h"

struct voxel;
class Content;

class SpaceWorldGenerator : WorldGenerator {
public:

	SpaceWorldGenerator(const Content* content) : WorldGenerator(content) {}

	void generate(voxel* voxels, int x, int z, int seed);
};

#endif /* VOXELS_SPACEWORLDGENERATOR_H_ */