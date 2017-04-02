#include "unit.h"

using namespace capacity_planning;

unit::unit()
	: _part(nullptr)
{
}


unit::unit(int id, part * const p, int due_date)
	: _id(id), _part(p), _due_date(due_date)
{
}


unit::~unit()
{
}
