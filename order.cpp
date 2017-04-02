#include "order.h"


using namespace capacity_planning;

order::order()
{
}

order::order(int id, int part_id, float quantity, int due_date)
	: _id(id), _part_id(part_id), _quantity(quantity), _due_date(due_date)
{
}


order::~order()
{
}
