#include "TerrainGenerator.h"
#include "../Noise.h"
#include "../PerlinNoise.hpp"
#include <iostream>
#include <cmath>

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

inline void SetNorth(std::vector<float>* heights, int sideSize, Vector2 p, float value)
{
	if (p.y != 0)
	{
		(*heights)[linearize(p.x, p.y - 1, sideSize)] = value;
	}
}

inline void SetSouth(std::vector<float>* heights, int sideSize, Vector2 p, float value)
{
	if (p.y != sideSize)
	{
		(*heights)[linearize(p.x, p.y + 1, sideSize)] = value;
	}
}

inline void SetWest(std::vector<float>* heights, int sideSize, Vector2 p, float value)
{
	if (p.x != 0)
	{
		(*heights)[linearize(p.x - 1, p.y, sideSize)] = value;
	}
}

inline void SetEast(std::vector<float>* heights, int sideSize, Vector2 p, float value)
{
	if (p.x != sideSize)
	{
		(*heights)[linearize(p.x + 1, p.y, sideSize)] = value;
	}
}

std::pair<Vector2, Vector2> CalculateGradient(std::vector<float>* heights, int sideSize, Vector2 p)
{
	// east - west, south - north
	int index = linearize(p.x, p.y, sideSize);
	Vector2 positiveGradient = { QueryEast(heights, sideSize, p) - (*heights)[index], QuerySouth(heights, sideSize, p) - (*heights)[index] };
	Vector2 negativeGradient = { QueryWest(heights, sideSize, p) - (*heights)[index], QueryNorth(heights, sideSize, p) - (*heights)[index] };
	return std::make_pair(positiveGradient, negativeGradient);
}

void CalculateGradients(int sideSize, std::vector<float>* heights, std::vector<float>* heightsPlateau, std::vector<std::pair<Vector2, Vector2>>* gradients, std::vector<std::pair<Vector2, Vector2>>* gradientsPlateau)
{
	gradients->clear();
	gradientsPlateau->clear();
	for (int y = 0; y < sideSize; y++)
	{
		for (int x = 0; x < sideSize; x++)
		{
			Vector2 p = { (float)x, (float)y };
			std::pair<Vector2, Vector2> g = CalculateGradient(heightsPlateau, sideSize, p);
			gradientsPlateau->emplace_back(g);
			g = CalculateGradient(heights, sideSize, p);
			gradients->emplace_back(g);
		}
	}
}

std::vector<Color32> TerrainGenerator::Generate(int sideSize, float displacement)
{
	colors.clear();
	heights.clear();
	heightsPlateau.clear();
	siv::BasicPerlinNoise<float> noise;
	noise.reseed(1337);

	for (int y = 0; y < sideSize; y++)
	{
		for (int x = 0; x < sideSize; x++)
		{
			float nx = 2.f * x / sideSize - 1;
			float ny = 2.f * y / sideSize - 1;
			float d = 1 - (1 - nx * nx) * (1 - ny * ny);
			float n = noise.octave3D(x / 50.0, y / 50.0, displacement, octaves);
			float e = std::clamp((1 + n) / 2.f, 0.f, 1.f) * 255;
			e = (e + ((1 - d) * 255.f - 1.f)) / 2.f;
			heights.emplace_back(e);

			for (int i = 0; i < heightColors.size(); ++i)
			{
				if (e >= heightColors[i].first)
				{
					heightsPlateau.emplace_back(heightColors[i].first);
					break;
				}
			}
		}
	}

	CalculateGradients(sideSize, &heights, &heightsPlateau, &gradients, &gradientsPlateau);

	UpdateColors(sideSize, displacement);
	return colors;
}

float ConvolveStep(std::vector<float>* heights, std::vector<std::pair<Vector2, Vector2>>* gradients, int sideSize, Vector2 p, int convolutionType, float incAmount)
{
	switch (convolutionType)
	{
	case 0: // Increase
		return std::clamp((*heights)[linearize(p.x, p.y, sideSize)] + incAmount, 0.f, 255.f);
	case 1: // Invert
		return 255.f - (*heights)[linearize(p.x, p.y, sideSize)];
	case 2: // Thermal Erosion
		return (QueryEast(heights, sideSize, p) + QueryWest(heights, sideSize, p) + QuerySouth(heights, sideSize, p) + QueryNorth(heights, sideSize, p)) / 4.f;
	case 3: // Hydraulic Erosion
		int index = linearize(p.x, p.y, sideSize);				// The index of the current pixel in the image
		std::pair<Vector2, Vector2> g = (*gradients)[index];	// The gradient for each direction of that pixel (E S W N)
		float waterVolume = 0.0f;								// The amount of water on the current pixel
		float materialVolume = 0.0f;							// The amount of material in the water
		float pMaterialVolume = materialVolume;					// The previous amount of material
		float waterFlow[4] = {0.0f, 0.0f, 0.0f, 0.0f};			// The amount of water that will flow in each direction
		Vector2 waterVelocity = {0.0f, 0.0f};					// The overall velocity of the water

		// Simulate rainfall
		waterVolume++;
	}
	return (*heights)[linearize(p.x, p.y, sideSize)];
}

std::vector<Color32> TerrainGenerator::Convolve(int sideSize, float displacement)
{
	if (heights.size() > 0)
	{
		for (int i = 0; i < convolveSteps; ++i)
		{
			std::vector<float> newHeights;
			std::vector<float> newHeightsPlateau;
			for (int y = 0; y < sideSize; y++)
			{
				for (int x = 0; x < sideSize; x++)
				{
					Vector2 p = { (float)x, (float)y };
					int index = linearize(p.x, p.y, sideSize);
					float e;
					if (plateau)
						e = ConvolveStep(&heights, &gradientsPlateau, sideSize, p, convolutionType, conIncreaseAmount);
					else
						e = ConvolveStep(&heights, &gradients, sideSize, p, convolutionType, conIncreaseAmount);
					newHeights.emplace_back(e);

					for (int i = 0; i < heightColors.size(); ++i)
					{
						if (e >= heightColors[i].first)
						{
							newHeightsPlateau.emplace_back(heightColors[i].first);
							break;
						}
					}
				}
			}

			for (int i = 0; i < sideSize * sideSize; ++i)
			{
				heights[i] = newHeights[i];
				heightsPlateau[i] = newHeightsPlateau[i];
			}

			CalculateGradients(sideSize, &heights, &heightsPlateau, &gradients, &gradientsPlateau);
		}
		UpdateColors(sideSize, displacement);
		return colors;
	}

	for (int i = 0; i < sideSize * sideSize; ++i)
	{
		colors.emplace_back(Color32(0, 0, 0));
	}
	return colors;
}

std::vector<Color32> TerrainGenerator::UpdateColors(int sideSize, float displacement)
{
	colors.clear();
	for (int y = 0; y < sideSize; y++)
	{
		for (int x = 0; x < sideSize; x++)
		{
			Vector2 p = { (float)x, (float)y };
			int index = linearize(p.x, p.y, sideSize);
			if (visualizeGradient)
			{
				if (plateau)
					if (positiveGradient)
						colors.emplace_back(Color32((255.f + gradientsPlateau[index].first.x) / 2.f, (255.f + gradientsPlateau[index].first.y) / 2.f, 127));
					else
						colors.emplace_back(Color32((255.f + gradientsPlateau[index].second.x) / 2.f, (255.f + gradientsPlateau[index].second.y) / 2.f, 127));
				else
					if (positiveGradient)
						colors.emplace_back(Color32((255.f + gradients[index].first.x) / 2.f, (255.f + gradients[index].first.y) / 2.f, 127));
					else
						colors.emplace_back(Color32((255.f + gradients[index].second.x) / 2.f, (255.f + gradients[index].second.y) / 2.f, 127));
			}
			else
			{
				for (int i = 0; i < heightColors.size(); ++i)
				{
					float e = heights[index];
					if (e >= heightColors[i].first)
					{
						if (greyscale)
							colors.emplace_back(Color32(e, e, e));
						else
							colors.emplace_back(heightColors[i].second);
						break;
					}
				}
			}
		}
	}
	return colors;
}

std::string TerrainGenerator::GetName() { return "Terrain Generator"; }
