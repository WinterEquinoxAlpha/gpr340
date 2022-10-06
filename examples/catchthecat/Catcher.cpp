#include "Catcher.h"
#include "World.h"

Point2D Catcher::Move(World* world) {
	std::unordered_map<int, std::unordered_map<int, bool>> visited;
	std::unordered_map<int, std::unordered_map<int, Point2D>> from;
	std::vector<Point2D> currentNeighbours;

	visited.clear();
	from.clear();
	currentNeighbours.clear();

	Point2D solution = Point2D(0, 0);

	std::vector<queueEntry> queue;
	queue.push_back({ world->getCat(), 0 });
	while (!queue.empty())
	{
		std::sort(queue.begin(), queue.end());
		// remove the head
		queueEntry head = queue[0];
		queue.erase(queue.begin());
		// mark the head as visited
		visited[head.position.x][head.position.y] = true;

		// for each neighbour
		currentNeighbours.push_back(World::NE(head.position));
		currentNeighbours.push_back(World::NW(head.position));
		currentNeighbours.push_back(World::E(head.position));
		currentNeighbours.push_back(World::W(head.position));
		currentNeighbours.push_back(World::SW(head.position));
		currentNeighbours.push_back(World::SE(head.position));
		for (Point2D n : currentNeighbours)
		{
			// if it's the edge, exit the loop
			if (std::abs(n.x) > world->getWorldSideSize() / 2 || std::abs(n.y) > world->getWorldSideSize() / 2)
			{
				solution = head.position;
				while (world->getContent(head.position))
				{
					solution = from[solution.x][solution.y];
				}
				return solution;
			}

			// if it's not visited, nor blocked
			if (!world->getContent(n) && !visited[n.x][n.y])
			{
				// add it to the queue
				// mark its weight as 1 more than the current weight
				queue.push_back({ n, head.weight + 1 });

				// mark where this element is from
				from[n.x][n.y] = head.position;
			}
		}
		currentNeighbours.clear();
	}
	return solution;
}
