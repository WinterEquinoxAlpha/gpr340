#ifndef MOBAGEN_TERRAINGENERATOR_H
#define MOBAGEN_TERRAINGENERATOR_H

#include "../GeneratorBase.h"
#include "Vector2.h"

class TerrainGenerator : public ScenarioGeneratorBase
{
public:
	std::vector<Color32> Generate(int sideSize, float displacement = 0) override;
	std::vector<Color32> Convolve(int sideSize, float displacement = 0) override;
	std::vector<Color32> UpdateColors(int sideSize, float displacement = 0) override;
	std::string GetName() override;
	std::vector<Color32> colors;
	std::vector<float> heights;
	std::vector<float> heightsPlateau;
	std::vector<std::pair<Vector2, Vector2>> gradients;
	std::vector<std::pair<Vector2, Vector2>> gradientsPlateau;

    std::vector<std::pair<int, Color32>> heightColors =
    {
        {230, Color32(255, 250, 250)},
        {220, Color32(244, 240, 240)},
        {210, Color32(222, 220, 220)},
        {200, Color32(215, 215, 215)},
        {190, Color32(255, 198, 153)},
        {180, Color32(245, 190, 149)},
        {175, Color32(220, 180, 143)},
        {170, Color32(200, 170, 133)},
        {160, Color32(0, 154, 23)},
        {140, Color32(0, 130, 23)},
        {120, Color32(0, 110, 23)},
        {100, Color32(0, 80, 23)},
        {90, Color32(95, 187, 218)},
        {40, Color32(65, 167, 218)},
        {0, Color32(35, 137, 218)}
    };
};

#endif  // MOBAGEN_TERRAINGENERATOR_H
