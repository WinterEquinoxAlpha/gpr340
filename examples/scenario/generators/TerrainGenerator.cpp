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

std::vector<float> CalculateGradient(std::vector<float>* heights, int sideSize, Vector2 p)
{
	int index = linearize(p.x, p.y, sideSize);
	// Positive gradients
	float e = QueryEast(heights, sideSize, p) - (*heights)[index];
	float s = QuerySouth(heights, sideSize, p) - (*heights)[index];
	// Negative gradients
	float w = QueryWest(heights, sideSize, p) - (*heights)[index];
	float n = QueryNorth(heights, sideSize, p) - (*heights)[index];
	return std::vector<float> {e, s, w, n};
}

void CalculateGradients(int sideSize, std::vector<float>* heights, std::vector<float>* heightsPlateau, std::vector<std::vector<float>>* gradients, std::vector<std::vector<float>>* gradientsPlateau)
{
	gradients->clear();
	gradientsPlateau->clear();
	for (int y = 0; y < sideSize; y++)
	{
		for (int x = 0; x < sideSize; x++)
		{
			Vector2 p = { (float)x, (float)y };
			//std::cout << "p: " << p.x << "," << p.y << "\n";
			std::vector<float> g = CalculateGradient(heightsPlateau, sideSize, p);
			//std::cout << "plateau g: " << g[0] << "," << g[1] << "," << g[2] << "," << g[3] << "\n";
			gradientsPlateau->emplace_back(g);
			g = CalculateGradient(heights, sideSize, p);
			//std::cout << "normal g: " << g[0] << "," << g[1] << "," << g[2] << "," << g[3] << "\n";
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

float ConvolveStep(std::vector<float>* heights,
				   std::vector<std::vector<float>>* gradients,
				   int sideSize,
				   Vector2 p,
				   int convolutionType,
				   float incAmount,
				   std::vector<float>* waterVolume,
				   std::vector<float>* materialVolume,
				   std::vector<float>* pMaterialVolume,
				   std::vector<std::vector<float>>* waterFlow)
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
		int index = linearize(p.x, p.y, sideSize);								// The index of the current pixel in the image
		std::vector<float> g = (*gradients)[index];								// The elevation gradient for each direction of that pixel (E S W N)
		std::vector<float> wg = CalculateGradient(waterVolume, sideSize, p);	// The water gradient for each direction of that pixel (E S W N)
		float newHeight = (*heights)[index];									// The new elevation the function will return

		Vector2 waterVelocity;			// The overall velocity of the water
		float rainfall = 1.0f;			// The amout of water that is added each step
		float flowRate = 0.5f;			// The rate of flow of water from one pixel to another
		float pickThreshold = 1.0f;		// The speed at which water will start to pick up material
		float dropThreshold = 1.0f;		// The speed at which water will start to drop off material
		float solubility = 1.0f;		// Ease of the material to be picked up
		float evaporation = 0.9f;		// Percent of water that evaporates

		//std::cout << "Beginning: " << (*materialVolume)[index] << "\n";

		// Step 1: Simulate rainfall
		(*waterVolume)[index] += rainfall;

		// Step 2: Calculate the total water flow
		float totalFlow = 0;
		for (int i = 0; i < 4; ++i)
		{
			//std::cout << "----------------- Calculate Flow Loop -----------------" << "\n";
			float dh = g[i];
			float dw = wg[i];
			//std::cout << "dh: " << dh << "\n";
			//std::cout << "dw: " << dw << "\n";
			(*waterFlow)[index][i] += flowRate * (dh + dw);
			//std::cout << "waterFlow 1: " << (*waterFlow)[index][i] << "\n";
			(*waterFlow)[index][i] = std::max(0.0f, (*waterFlow)[index][i]);
			//std::cout << "waterFlow 2: " << (*waterFlow)[index][i] << "\n";
			totalFlow += (*waterFlow)[index][i];
			//std::cout << "totalFlow: " << totalFlow << "\n";
		}
		if (totalFlow > (*waterVolume)[index])
		{
			float prop = (*waterVolume)[index] / totalFlow;
			prop = std::max(0.0f, prop);
			for (int i = 0; i < 4; ++i)
			{
				(*waterFlow)[index][i] *= prop;
			}
		}
		float hOut = (*waterFlow)[index][0] - (*waterFlow)[index][2];
		float hIn;
		if (p.x > 0 && p.x < sideSize - 1)
			hIn = (*waterFlow)[linearize(p.x + 1, p.y, sideSize)][2] - (*waterFlow)[linearize(p.x - 1, p.y, sideSize)][0];
		else
			hIn = (*waterFlow)[index][2] - (*waterFlow)[index][0];

		float vOut = (*waterFlow)[index][1] - (*waterFlow)[index][3];
		float vIn;
		if (p.y > 0 && p.y < sideSize - 1)
			vIn = (*waterFlow)[linearize(p.x, p.y + 1, sideSize)][3] - (*waterFlow)[linearize(p.x, p.y - 1, sideSize)][1];
		else
			vIn = (*waterFlow)[index][3] - (*waterFlow)[index][1];
		
		waterVelocity = {hOut + hIn, vOut + vIn};

		// Step 3: Calculate movement of material
		float waterSpeed = waterVelocity.getMagnitude();
		if (waterSpeed > pickThreshold)
		{
			float pick = waterSpeed * solubility;
			(*materialVolume)[index] += pick;
			//std::cout << "waterSpeed > pickThreshold: " << (*materialVolume)[index] << "\n";
			newHeight -= pick;
		}
		else if (waterSpeed < dropThreshold)
		{
			float prop = (dropThreshold - waterSpeed) / dropThreshold;
			float drop = prop * (*materialVolume)[index];
			//std::cout << "WS: " << waterSpeed << ", P: " << prop << ", D: " << drop << "\n";
			(*materialVolume)[index] -= drop;
			//std::cout << "waterSpeed < pickThreshold: " << (*materialVolume)[index] << "\n";
			newHeight += drop;
		}

		// Save the material info
		(*pMaterialVolume)[index] = (*materialVolume)[index];
		//std::cout << "pMaterialVolume: " << (*pMaterialVolume)[index] << "\n";

		// Step 4: Move the water
		float directions[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
		for (int i = 0; i < 4; ++i)
		{
			float currentWaterFlow = (*waterFlow)[index][i];
			float prop = currentWaterFlow / totalFlow;
			prop = std::max(0.0f, prop);
			float materialFlow = prop * (*pMaterialVolume)[index];

			//std::cout << "----------------- Move Water Loop -----------------" << "\n";
			//std::cout << "currentWaterFlow: " << currentWaterFlow << "\n";
			//std::cout << "totalFlow: " << totalFlow << "\n";
			//std::cout << "prop: " << prop << "\n";
			//std::cout << "materialFlow: " << materialFlow << "\n";

			Vector2 dd = {p.x + directions[i][0], p.y + directions[i][1]};
			if (dd.x < 0 || dd.x >= sideSize)
				dd.x = p.x;
			if (dd.y < 0 || dd.y >= sideSize)
				dd.y = p.y;

			(*waterVolume)[index] -= currentWaterFlow;
			(*waterVolume)[linearize(dd.x, dd.y, sideSize)] += currentWaterFlow;

			(*materialVolume)[index] -= materialFlow;
			//std::cout << "-= materialFlow: " << (*materialVolume)[index] << "\n";
			(*materialVolume)[linearize(dd.x, dd.y, sideSize)] += materialFlow;
			//std::cout << "+= materialFlow: " << (*materialVolume)[index] << "\n";
		}

		// Step 5: Evaporate water
		(*waterVolume)[index] *= (1 - evaporation);

		//std::cout << "End: " << (*materialVolume)[index] << "\n";
		//std::cout << "currentHeight: " << (*heights)[index] << ", newHeight: " << newHeight << "\n\n";

		return newHeight;
	}
	return (*heights)[linearize(p.x, p.y, sideSize)];
}

std::vector<Color32> TerrainGenerator::Convolve(int sideSize, float displacement)
{
	if (heights.size() > 0)
	{
		for (int i = 0; i < convolveSteps; ++i)
		{
			// For hydraulic erosion
			std::vector<float> waterVolume;				// The amount of water on the current pixel
			std::vector<float> materialVolume;			// The amount of material in the water
			std::vector<float> pMaterialVolume;			// The previous amount of material
			std::vector<std::vector<float>> waterFlow;	// The amount of water that will flow in each direction

			for (int i = 0; i < heights.size(); ++i)
			{
				waterVolume.emplace_back(0.0f);
				materialVolume.emplace_back(0.0f);
				pMaterialVolume.emplace_back(0.0f);
				waterFlow.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
			}
			
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
						e = ConvolveStep(&heights, &gradientsPlateau, sideSize, p, convolutionType, conIncreaseAmount, &waterVolume, &materialVolume, &pMaterialVolume, &waterFlow);
					else
						e = ConvolveStep(&heights, &gradients, sideSize, p, convolutionType, conIncreaseAmount, &waterVolume, &materialVolume, &pMaterialVolume, &waterFlow);
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

			for (int i = 0; i < heights.size(); ++i)
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
						colors.emplace_back(Color32((255.f + gradientsPlateau[index][0]) / 2.f, (255.f + gradientsPlateau[index][1]) / 2.f, 127));
					else
						colors.emplace_back(Color32((255.f + gradientsPlateau[index][2]) / 2.f, (255.f + gradientsPlateau[index][3]) / 2.f, 127));
				else
					if (positiveGradient)
						colors.emplace_back(Color32((255.f + gradients[index][0]) / 2.f, (255.f + gradients[index][1]) / 2.f, 127));
					else
						colors.emplace_back(Color32((255.f + gradients[index][2]) / 2.f, (255.f + gradients[index][3]) / 2.f, 127));
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
