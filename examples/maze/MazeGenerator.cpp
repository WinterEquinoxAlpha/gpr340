#include "MazeGenerator.h"
#include "World.h"
#include "Random.h"

bool MazeGenerator::Step(World* world) {
	int worldSize = world->GetSize()/2;
	std::vector<Node> from;
	Node currentNode = world->GetNode(currentPoint);
	std::vector<Point2D> currentNeighbours;

	currentNeighbours.clear();
	if (currentNode.GetEast())
	{
		currentNeighbours.push_back(Point2D(1,0));
	}
	if (currentNode.GetSouth())
	{
		currentNeighbours.push_back(Point2D(0, 1));
	}
	if (currentNode.GetWest())
	{
		currentNeighbours.push_back(Point2D(-1, 0));
	}
	if (currentNode.GetNorth())
	{
		currentNeighbours.push_back(Point2D(0, -1));
	}

	if (currentNeighbours.size() != 0)
	{
		int r = Random::Range(0, currentNeighbours.size() - 1);
		currentNode.ClearNode();
		world->SetNode(currentPoint, currentNode);
		currentPoint += currentNeighbours[r];
	}
	
	/*for (int y = -worldSize; y <= worldSize; ++y)
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
	}*/
	
	return false;
}
void MazeGenerator::Clear(World* world) {
	currentPoint = { 0, 0 };
}
