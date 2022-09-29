#include "Cat.h"
#include "World.h"
#include <stdexcept>
#include <algorithm>

Point2D Cat::Move(World* world)
{
	visited.clear();
	from.clear();

	std::vector<queueEntry> queue;
	queue.push_back({world->getCat(), 0});
	while (!queue.empty())
	{
		std::sort(queue.begin(), queue.end());
		// remove the head
		queueEntry head = queue[0];
        queue.erase(queue.begin());
		// mark the head as visited
        
		// for each neighbour you have to
		// if it's not visited, nor the block...
		// add to the queue w 1 unit of the weight increased
		// mark where this element is from
	}

	auto rand = Random::Range(0,5);
	auto pos = world->getCat();
	switch(rand)
	{
	case 0:
		return World::NE(pos);
	case 1:
		return World::NW(pos);
	case 2:
		return World::E(pos);
	case 3:
		return World::W(pos);
	case 4:
		return World::SW(pos);
	case 5:
		return World::SE(pos);
	default:
		throw "random out of range";
	}
}
