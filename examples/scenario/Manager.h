#ifndef MANAGER_H
#define MANAGER_H

#include "ColorT.h"
#include "GameObject.h"
#include "GeneratorBase.h"
#include <SDL.h>

class Manager: public GameObject {
 private:
  float accumulatedTime = 0;
  int sideSize=5;
  SDL_Texture *texture;
  bool isSimulating = false;
  bool isConvoluting = false;

  std::vector<ScenarioGeneratorBase*> generators;
  int generatorId=0;

  void step();
  void convolve();
  void updateColors();
 public:
  ~Manager();
  explicit Manager(Engine* engine, int size);

  void Start() override;
  void OnGui(ImGuiContext *context) override;
  void OnDraw(SDL_Renderer* renderer) override;
  void Update(float deltaTime) override;

  void Clear();
  int GetSize() const;

  void SetPixels(std::vector<Color32> &pixels);
};

#endif  // MANAGER_H
