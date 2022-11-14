#ifndef MOBAGEN_TERRAINGENERATOR_H
#define MOBAGEN_TERRAINGENERATOR_H

#include "../GeneratorBase.h"

class TerrainGenerator : public ScenarioGeneratorBase
{
public:
	std::vector<Color32> Generate(int sideSize, float displacement = 0) override;
	std::string GetName() override;
};

#endif  // MOBAGEN_TERRAINGENERATOR_H
