#ifndef SCENARIOGENERATORBASE_H
#define SCENARIOGENERATORBASE_H
#include <ColorT.h>
#include <string>
#include <vector>

class ScenarioGeneratorBase {
 public:
  virtual std::vector<Color32> Generate(int sideSize, float displacement=0)=0;
  virtual std::string GetName()=0;
  int octaves = 2;
  bool visualizeGradient = false;
  bool greyscale = true;
};

#endif  // SCENARIOGENERATORBASE_H
