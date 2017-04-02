#pragma once

#include "unit.h"

#include <vector>

namespace capacity_planning
{
	class resource
	{
	public:
		resource();
		resource(resource * const resource);
		resource(int id);
		~resource();

		int get_id() { return _id; }

		inline void push_back(unit *u) { units.push_back(u); }

		void clear();

		std::vector<unit *> units;

	private:
		int _id;

	};
}

