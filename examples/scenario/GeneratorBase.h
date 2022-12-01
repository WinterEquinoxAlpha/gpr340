#ifndef SCENARIOGENERATORBASE_H
#define SCENARIOGENERATORBASE_H
#include <ColorT.h>
#include <string>
#include <vector>

class ScenarioGeneratorBase {
public:
	virtual std::vector<Color32> Generate(int sideSize, float displacement = 0) = 0;
	virtual std::vector<Color32> Convolve(int sideSize, float displacement = 0) = 0;
	virtual std::vector<Color32> UpdateColors(int sideSize, float displacement = 0) = 0;
	virtual std::string GetName() = 0;
	int octaves = 2;
	bool visualizeGradient = false;
	bool positiveGradient = true;
	bool greyscale = false;
	bool plateau = true;
	int convolveSteps = 1;
	float conIncreaseAmount = 0.1;
	int convolutionType = 3;
	std::vector<std::string> convolutionTypes = {
		"Average",
		"Increase",
		"Invert",
		"Gradient Descent"
	};
};

#endif  // SCENARIOGENERATORBASE_H
