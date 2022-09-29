#include "Agent.h"

bool queueEntry::operator<(const queueEntry& rhs) const
{
    return weight < rhs.weight;
}
