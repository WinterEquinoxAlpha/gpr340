#include "MazeGenerator.h"
#include "World.h"
#include "Random.h"

bool MazeGenerator::Step(World* world) {
	int worldSize = world->GetSize()/2;
	std::vector<Node> from;
	Node currentNode = world->GetNode(currentPoint);
	std::vector<std::pair<Point2D,int>> currentNeighbours;

	currentNeighbours.clear();
	if (currentNode.GetEast() && currentPoint.x < worldSize)
	{
		currentNeighbours.push_back(std::pair(Point2D(1,0), 0));
	}
	if (currentNode.GetSouth() && currentPoint.y < worldSize)
	{
		currentNeighbours.push_back(std::pair(Point2D(0, 1), 1));
	}
	if (currentNode.GetWest() && currentPoint.x > -worldSize)
	{
		currentNeighbours.push_back(std::pair(Point2D(-1, 0), 2));
	}
	if (currentNode.GetNorth() && currentPoint.y > -worldSize)
	{
		currentNeighbours.push_back(std::pair(Point2D(0, -1), 3));
	}
	std::cout << currentNeighbours.size() << std::endl;

	if (currentNeighbours.size() != 0)
	{
		int r = Random::Range(0, currentNeighbours.size() - 1);
		switch (currentNeighbours[r].second)
		{
			case 0:
				currentNode.SetEast(false);
				break;
			case 1:
				currentNode.SetSouth(false);
				break;
			case 2:
				currentNode.SetWest(false);
				break;
			case 3:
				currentNode.SetNorth(false);
				break;
		}
		world->SetNode(currentPoint, currentNode);
		world->SetNodeColor(currentPoint, Color::Red.Dark());
		currentPoint += currentNeighbours[r].first;
		world->SetNodeColor(currentPoint, Color::Green);
	}

	/*
	if (currentNeighbours.size() != 0)
	{
		int r = Random::Range(0, currentNeighbours.size() - 1);
		currentNode.ClearNode();
		world->SetNode(currentPoint, currentNode);
		currentPoint += currentNeighbours[r];
	}*/
	
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
