#include "Catcher.h"
#include "World.h"
#include <set>

Point2D Catcher::Move(World* world)
{
	std::unordered_map<int, std::unordered_map<int, bool>> visited;
	std::unordered_map<int, std::unordered_map<int, Point2D>> from;
	std::vector<queueEntry> queue;
	std::vector<Point2D> currentNeighbours;

	visited.clear();
	from.clear();
	queue.clear();

	queue.push_back({ world->getCat(), 0 });
	while (!queue.empty())
	{
		// sort the vector
		std::sort(queue.begin(), queue.end());

		// fetch the head and erase it from the queue
		queueEntry head = *queue.begin();
		queue.erase(queue.begin());

		// mark the head as visited
		visited[head.position.x][head.position.y] = true;

		// get each neighbour
		currentNeighbours.clear();
		currentNeighbours.push_back(World::NE(head.position));
		currentNeighbours.push_back(World::NW(head.position));
		currentNeighbours.push_back(World::E(head.position));
		currentNeighbours.push_back(World::W(head.position));
		currentNeighbours.push_back(World::SW(head.position));
		currentNeighbours.push_back(World::SE(head.position));

		for (Point2D n : currentNeighbours)
		{
			// if the neighbour being checked is outside the world, then we have reached the end
			if (!world->isValidPosition(n))
			{
				return head.position;
			}

			// if the neighbour is blocked, don't add it
			if (world->getContent(n))
			{
				continue;
			}

			// if it's not visited and not already in the queue
			queueEntry newEntry = { n, head.weight + 1 };
			if (!visited[n.x][n.y] && std::find(queue.begin(), queue.end(), newEntry) == queue.end())
			{
				// add it to the queue and mark its weight as 1 more than the current weight
				queue.push_back(newEntry);

				// mark where this element is from
				from[n.x][n.y] = head.position;
			}
		}
	}

	Point2D pos = world->getCat();
	currentNeighbours.clear();
	currentNeighbours.push_back(World::NE(pos));
	currentNeighbours.push_back(World::NW(pos));
	currentNeighbours.push_back(World::E(pos));
	currentNeighbours.push_back(World::W(pos));
	currentNeighbours.push_back(World::SW(pos));
	currentNeighbours.push_back(World::SE(pos));
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(currentNeighbours.begin(), currentNeighbours.end(), g);
	for (int i = currentNeighbours.size() - 1; i >= 0; --i)
	{
		if (world->getContent(currentNeighbours[i]))
		{
			currentNeighbours.erase(currentNeighbours.begin() + i);
		}
	}
	if (currentNeighbours.size() > 0)
	{
		return currentNeighbours[0];
	}

	// if all else has failed, return 0,0
	return Point2D(0, 0);
}
