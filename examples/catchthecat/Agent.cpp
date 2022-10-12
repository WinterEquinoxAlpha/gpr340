#include "Agent.h"

bool queueEntry::operator < (const queueEntry& rhs) const
{
    return weight < rhs.weight;
}

bool queueEntry::operator == (const queueEntry& rhs) const
{
    return position == rhs.position;
}

std::ostream& operator << (std::ostream& os, const queueEntry& qe)
{
    os << "X: " << qe.position.x << ", Y: " << qe.position.y << ", W: " << qe.weight;
    return os;
}