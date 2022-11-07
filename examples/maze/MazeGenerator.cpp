#include "MazeGenerator.h"
#include "World.h"
#include "Random.h"

bool MazeGenerator::Step(World* world) {
	int worldSize = world->GetSize()/2;
	std::vector<Point2D> currentNeighbours;

	if (newStart)
	{
		newStart = false;
		currentPoint = Point2D(Random::Range(-worldSize, worldSize), Random::Range(-worldSize, worldSize));
	}

	visited[currentPoint.x][currentPoint.y] = true;

	currentNeighbours.clear();
	if (currentPoint.x < worldSize && !visited[currentPoint.x + 1][currentPoint.y] && world->GetEast(currentPoint))
	{
		currentNeighbours.push_back(Point2D(1, 0));
	}
	if (currentPoint.x > -worldSize && !visited[currentPoint.x - 1][currentPoint.y] && world->GetWest(currentPoint))
	{
		currentNeighbours.push_back(Point2D(-1, 0));
	}
	if (currentPoint.y < worldSize && !visited[currentPoint.x][currentPoint.y + 1] && world->GetSouth(currentPoint))
	{
		currentNeighbours.push_back(Point2D(0, 1));
	}
	if (currentPoint.y > -worldSize && !visited[currentPoint.x][currentPoint.y - 1] && world->GetNorth(currentPoint))
	{
		currentNeighbours.push_back(Point2D(0, -1));
	}

	if (currentNeighbours.size() != 0)
	{
		int r = Random::Range(0, currentNeighbours.size() - 1);
		
		if (currentNeighbours[r].x == 1)
		{
			world->SetEast(currentPoint, false);
		}
		if (currentNeighbours[r].x == -1)
		{
			world->SetWest(currentPoint, false);
		}
		if (currentNeighbours[r].y == 1)
		{
			world->SetSouth(currentPoint, false);
		}
		if (currentNeighbours[r].y == -1)
		{
			world->SetNorth(currentPoint, false);
		}

		world->SetNodeColor(currentPoint, Color::Blue.Light());
		previousPoints.push_back(currentPoint);
		currentPoint += currentNeighbours[r];
		world->SetNodeColor(currentPoint, Color::Green.Light());
	}
	else
	{
		if (previousPoints.empty())
		{
			world->SetNodeColor(currentPoint, Color::Black);
			newStart = true;
			return false;
		}
		world->SetNodeColor(currentPoint, Color::Black);
		currentPoint = previousPoints.back();
		previousPoints.pop_back();
		world->SetNodeColor(currentPoint, Color::Green.Light());
	}
	
	return true;
}

void MazeGenerator::Clear(World* world) {
	int worldSize = world->GetSize() / 2;
	newStart = true;
	visited.clear();
	previousPoints.clear();
}
