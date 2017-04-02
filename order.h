#pragma once

#include <vector>

namespace capacity_planning
{
	class order
	{

	public:
		order();
		order(int id, int _part_id, float quantity, int due_date);
		~order();

		float get_quantity() { return _quantity; }
		int get_part_id() { return _part_id; }
		inline int get_id() { return _id; }

	private:
		int _id;
		int _part_id;
		float _quantity;
		int _due_date;

	};
}

