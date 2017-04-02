#pragma once

#include <vector>

namespace capacity_planning
{
	class part
	{
	public:
		part();
		part(int id,
			 float unit_size,
			 short batch_size,
			 short batch_runtime_minutes,
			 std::vector<unsigned char> possible_machines);
		~part();

		inline short get_batch_runtime_minutes() { return _batch_runtime_minutes; }
		inline float get_batch_size() { return _batch_size; }
		inline float get_unit_size() { return _unit_size; }
		inline int get_id() { return _id; }

		std::vector<unsigned char> get_possible_machines() { return _possible_machines; }
	private:
		int _id;
		float _unit_size;
		float _batch_size;
		short _batch_runtime_minutes;
		std::vector<unsigned char> _possible_machines;
	};
}

