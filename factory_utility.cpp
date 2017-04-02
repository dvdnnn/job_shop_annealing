#include "factory_utility.h"

#include <algorithm>
#include <random>
#include <vector>


using namespace capacity_planning;
using namespace std;


factory_utility::factory_utility()
{
}


factory_utility::~factory_utility()
{
}


vector<int>
factory_utility::generate_resources(unordered_map<int, resource> &resources)
{
	vector<int> resource_ids;
	for (short i = 0; i < _NUMBER_OF_RESOURCES; ++i)
	{
		auto res = resource(i);
		resources[res.get_id()] = res;
		resource_ids.push_back(res.get_id());
	}
	return resource_ids;
}


vector<int>
factory_utility::generate_parts(unordered_map<int, part> &parts, mt19937_64 &mt)
{
	vector<int> part_ids;

	uniform_real_distribution<float> batch_dis(_BATCH_SIZE_MIN,
											   _BATCH_SIZE_MAX);

	uniform_int_distribution<int> runtime_dis(_RUNTIME_MIN,
											  _RUNTIME_MAX);

	uniform_real_distribution<float> unit_size_dis(_UNIT_SIZE_MIN,
												   _UNIT_SIZE_MAX);

	uniform_int_distribution<int> possible_machine_dis(1,
											           _NUMBER_OF_RESOURCES);

	vector<int> resource_indices({ 0, 1, 2, 3 });

	for (short i = 0; i < _NUMBER_OF_PARTS; ++i)
	{
		float unit_size = unit_size_dis(mt);

		char m_count = possible_machine_dis(mt);
		std::shuffle(resource_indices.begin(), resource_indices.end(), mt);

		vector<unsigned char> possible_machines;
		for (char i = 0; i < m_count; ++i)
		{
			possible_machines.push_back(resource_indices[i]);
		}
		auto p = part(i, unit_size, batch_dis(mt) * unit_size, runtime_dis(mt), possible_machines);
		parts[p.get_id()] = p;
		part_ids.push_back(p.get_id());
	}
	return part_ids;
}


vector<int> 
factory_utility::generate_orders(vector<int> &part_ids, unordered_map<int, order> &orders, mt19937_64 &mt)
{
	vector<int> order_ids;

	uniform_real_distribution<float> quantity_dis(_QUANTITY_MIN,
											      _QUANTITY_MAX);

	uniform_int_distribution<int> due_time_dis(5000, MAX_DUE_DATE);

	uniform_int_distribution<int> part_dis(0, _NUMBER_OF_PARTS - 1);


	for (int i = 0; i < _NUMBER_OF_ORDERS; ++i)
	{
		auto o = order::order(i, part_ids[part_dis(mt)], quantity_dis(mt), due_time_dis(mt));
		orders[o.get_id()] = o;
		order_ids.push_back(o.get_id());
	}

	return order_ids;
}

std::vector<int>
factory_utility::split_orders_to_units(vector<int> &order_ids,
									   unordered_map<int, order> &orders,
								       unordered_map<int, unit> &units,
									   unordered_map<int, part> &parts)
{
	vector<int> index_container;
	int iter = 0;
	for (size_t i = 0; i < order_ids.size(); ++i)
	{
		short iter(0);
		auto ord = orders[order_ids[i]];
		for (short j = 0; j < (short)(ord.get_quantity() / parts[ord.get_part_id()].get_unit_size()); ++j)
		{
			auto new_unit = unit(iter++, &parts[ord.get_part_id()], ord.get_id());
			units[new_unit.get_id()] = new_unit;
			index_container.push_back(new_unit.get_id());
		}
	}
	return index_container;
}