#include "MazeGenerator.h"
#include "World.h"

bool MazeGenerator::Step(World* world) {
	int worldSize = world->GetSize()/2;
	for (int y = -worldSize; y <= worldSize; ++y)
	{
		for (int x = -worldSize; x <= worldSize; ++x)
		{
			Point2D p = {x, y};
			world->SetNorth(p, false);
			world->SetWest(p, false);
		}
		world->SetEast({ worldSize, y }, false);
	}
	for (int x = -worldSize; x <= worldSize; ++x)
	{
		world->SetSouth({ x, worldSize }, false);
	}
	return false;
}
void MazeGenerator::Clear(World* world) {}
