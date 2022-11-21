#include "TerrainGenerator.h"
#include "../Noise.h"
#include "../PerlinNoise.hpp"
#include "Vector2.h"
#include <iostream>

inline int linearize(int x, int y, int size)
{
    return x + (y * size);
}

inline float QueryNorth(std::vector<float>* heights, int sideSize, Vector2 p)
{
    if (p.y == 0)
    {
        return (*heights)[linearize(p.x, p.y + 1, sideSize)];
    }
    return (*heights)[linearize(p.x, p.y - 1, sideSize)];
}

inline float QuerySouth(std::vector<float>* heights, int sideSize, Vector2 p)
{
    if (p.y == sideSize)
    {
        return (*heights)[linearize(p.x, p.y - 1, sideSize)];
    }
    return (*heights)[linearize(p.x, p.y + 1, sideSize)];
}

inline float QueryWest(std::vector<float>* heights, int sideSize, Vector2 p)
{
    if (p.x == 0)
    {
        return (*heights)[linearize(p.x + 1, p.y, sideSize)];
    }
    return (*heights)[linearize(p.x - 1, p.y, sideSize)];
}

inline float QueryEast(std::vector<float>* heights, int sideSize, Vector2 p)
{
    if (p.x == sideSize)
    {
        return (*heights)[linearize(p.x - 1, p.y, sideSize)];
    }
    return (*heights)[linearize(p.x + 1, p.y, sideSize)];
}

inline Vector2 CalculateGradient(std::vector<float>* heights, int sideSize, Vector2 p)
{
    // east - west, south - north
    Vector2 gradient = { QueryEast(heights, sideSize, p) - QueryWest(heights, sideSize, p), QuerySouth(heights, sideSize, p) - QueryNorth(heights, sideSize, p) };
    return gradient;
}

std::vector<Color32> TerrainGenerator::Generate(int sideSize, float displacement)
{
    std::vector<Color32> colors;
    std::vector<float> heights;
    siv::BasicPerlinNoise<float> noise;
    noise.reseed(1337);

    float rock = 200;
    float grass = 80;
    float water = 20;

    for (int y = 0; y < sideSize; y++)
    {
        for (int x = 0; x < sideSize; x++)
        {
            float nx = 2.f * x / sideSize - 1;
            float ny = 2.f * y / sideSize - 1;
            float d = 1 - (1 - nx * nx) * (1 - ny * ny);
            float e = abs(noise.octave3D(x / 50.0, y / 50.0, displacement, octaves) * 255);
            e += (1 - d) * 255 - 120;

            heights.emplace_back(e);

            if (greyscale)
            {
                colors.emplace_back(e, e, e);
            }
            else
            {
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
                else
                {
                    throw std::runtime_error("Height out of bounds");
                }
            }
        }
    }

    if (visualizeGradient)
    {
        for (int y = 0; y < sideSize; y++)
        {
            for (int x = 0; x < sideSize; x++)
            {
                Vector2 p = { (float)x, (float)y };
                Vector2 g = CalculateGradient(&heights, sideSize, p);
                float m = p.getMagnitude();
                //std::cout << m << "\n";
                colors[linearize(x, y, sideSize)] = Color32(m, m, m);
            }
        }
    }

    std::cout << colors.size() << std::endl;
    return colors;
}

std::string TerrainGenerator::GetName() { return "Terrain Generator"; }
