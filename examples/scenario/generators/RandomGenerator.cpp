#include "RandomGenerator.h"
#include "../Noise.h"
#include "../PerlinNoise.hpp"
#include <iostream>
// do not use this one to your assignment. this is my sample generator
std::vector<Color32> RandomScenarioGenerator::Generate(int sideSize, float displacement)
{
    std::vector<Color32> colors;
    //   create your own function for noise generation
    siv::BasicPerlinNoise<float> noise;
    noise.reseed(1337);
    //  Noise noise(1337, 1024,0,255);
    for (int l = 0; l < sideSize; l++)
    {
        for (int c = 0; c < sideSize; c++)
        {
            float rgb = ((1 + noise.octave3D(c / 50.0, l / 50.0, displacement, octaves)) / 2) * 255;
            colors.emplace_back(rgb, rgb, rgb);
            //      double color = noise.noise(c/50.0,l/50.0);
            //      colors.emplace_back(color,color,color);
        }
    }
    return colors;
}

std::vector<Color32> RandomScenarioGenerator::Convolve(int sideSize, float displacement)
{
    std::vector<Color32> colors;
    for (int i = 0; i < sideSize * sideSize; i++)
    {
        colors.emplace_back(Color32(0, 0, 0));
    }
    return colors;
}

std::vector<Color32> RandomScenarioGenerator::UpdateColors(int sideSize, float displacement)
{
    std::vector<Color32> colors;
    for (int i = 0; i < sideSize * sideSize; i++)
    {
        colors.emplace_back(Color32(0, 0, 0));
    }
    return colors;
}

std::string RandomScenarioGenerator::GetName() { return "EXAMPLE"; }
