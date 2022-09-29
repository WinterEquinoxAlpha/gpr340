#include "Catcher.h"
#include "World.h"

Point2D Catcher::Move(World* world) {
	int side = world->getWorldSideSize() * 0.5f;
	for(;;)
	{
		Point2D p = { Random::Range(-side, side), Random::Range(-side, side) };
		Point2D cat = world->getCat();
		if (cat.x != p.x && cat.y != p.y && !world->getContent(p))
		{
			return p;
		}
	}
}
