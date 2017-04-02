#pragma once

#include "part.h"

namespace capacity_planning
{
	class unit
	{

	public:
		unit();
		unit(int id, part * const p, int due_date);
		~unit();

		inline int get_id() { return _id; }
		inline part * const get_part() { return _part; }
		inline int get_due_date() { return _due_date; }

	private:
		int  _id;
		part * _part;
		int _due_date;
	};
}

