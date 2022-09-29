#ifndef AGENT_H
#define AGENT_H
#include "Point2D.h"
#include <unordered_map>

class World;

struct queueEntry
{
	Point2D position;
	int weight;
	bool operator < (const queueEntry& rhs) const;
};

class Agent
{
	public:
	explicit Agent()= default;
	virtual Point2D Move(World*)=0;
	std::unordered_map<int, std::unordered_map<int, bool>> visited;
	std::unordered_map<int, std::unordered_map<int, Point2D>> from;
};

#endif  // AGENT_H
