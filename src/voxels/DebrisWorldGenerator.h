#ifndef VOXELS_DEBRISWORLDGENERATOR_H_
#define VOXELS_DEBRISWORLDGENERATOR_H_

#include "../typedefs.h"
#include "../voxels/WorldGenerator.h"

struct voxel;
class Content;

class DebrisWorldGenerator : WorldGenerator {
public:

	DebrisWorldGenerator(const Content* content) : WorldGenerator(content) {}

	void generate(voxel* voxels, int x, int z, int seed);
};

#endif /* VOXELS_DEBRISWORLDGENERATOR_H_ */