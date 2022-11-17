#include "TerrainGenerator.h"
#include "../Noise.h"
#include "../PerlinNoise.hpp"
#include <iostream>

std::vector<Color32> TerrainGenerator::Generate(int sideSize, float displacement)
{
    std::vector<Color32> colors;
    //   create your own function for noise generation
    siv::BasicPerlinNoise<float> noise;
    noise.reseed(1337);

    float rock = 200;
    float grass = 80;
    float water = 20;

    //  Noise noise(1337, 1024,0,255);
    for (int l = 0; l < sideSize; l++)
    {
        for (int c = 0; c < sideSize; c++)
        {
            float e = abs(noise.octave3D(c / 50.0, l / 50.0, displacement, octaves) * 255);
            if (e > rock)
            {
                colors.emplace_back(255, 250, 250);
            }
            else if (e <= rock && e > grass)
            {
                colors.emplace_back(255, 198, 153);
            }
            else if (e <= grass && e > water)
            {
                colors.emplace_back(0, 154, 23);
            }
            else if (e <= water)
            {
                colors.emplace_back(35, 137, 218);
            }
        }
    }

    for (int i = 0; i < colors.size(); ++i)
    {
        colors[i] = colors[i].RandomColor();
        if (i % sideSize == sideSize / 3)
        {
            colors[i] = Color32(13,240,40);
        }
        
    }

    std::cout << colors.size() << std::endl;
    return colors;
}
std::string TerrainGenerator::GetName() { return "Terrain Generator"; }
