#include "part.h"

using namespace capacity_planning;

part::part()
{
}

part::part(int id,
		   float unit_size,
		   short batch_size,
		   short batch_runtime_minutes,
		   std::vector<unsigned char> possible_machines)
	: _id(id), _unit_size(unit_size), _batch_size(batch_size),
	  _batch_runtime_minutes(batch_runtime_minutes),
	  _possible_machines(possible_machines)
{
}


part::~part()
{
}