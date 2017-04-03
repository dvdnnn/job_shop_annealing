#include "factory_utility.h"

#include "timer.h"

#include <algorithm>
#include <cmath>
#include <iostream>


using namespace std;
using namespace capacity_planning;

#define SETUP_TIME 30

long double ef(vector<int> &resource_ids, unordered_map<int, resource> &resources)
{
	unsigned int cost = 0;
	for (int m_id : resource_ids)
	{
		auto m = resources[m_id];
		short time_used = 0;
		float alloted_available_batch = 0;
		short prev_part_id = m.units[0]->get_id();
		for (unit *u : m.units)
		{
			if (prev_part_id == u->get_part()->get_id()
				&& alloted_available_batch > 0)
			{
				alloted_available_batch -= 1;
			}
			else
			{
				time_used += u->get_part()->get_batch_runtime_minutes();
				if (u->get_part()->get_id() != prev_part_id)
				{
					time_used += SETUP_TIME;
				}
				alloted_available_batch = u->get_part()->get_batch_size();
			}
			short temp_cost = (time_used - u->get_due_date());
			cost += (temp_cost > 0 ? temp_cost : 0);
			prev_part_id = u->get_part()->get_id();
		}
	}
	return (long double)cost / 60 / 24;
};

auto random_function = [](vector<int> unit_ids, unordered_map<int, unit> &units,
			   vector<int> resource_ids, unordered_map<int, resource> &resources, mt19937_64 &mt)
{
	unordered_map<int, resource> new_resources;
	for (int r : resource_ids)
	{
		new_resources[r] = resource(r);
	}
	
	std::shuffle(unit_ids.begin(), unit_ids.end(), mt);
	std::shuffle(resource_ids.begin(), resource_ids.end(), mt);
	
	for (size_t i = 0; i < unit_ids.size(); ++i)
	{
		uniform_int_distribution<int> resource_dis(0, 
			(int)(units[unit_ids[i]].get_part()->get_possible_machines().size() - 1));

		new_resources[resource_ids[resource_dis(mt)]].push_back(&units[unit_ids[i]]);
	}

	return new_resources;
};

auto next_function = [](vector<int> unit_ids, unordered_map<int, unit> &units,
	vector<int> resource_ids, unordered_map<int, resource> resources, mt19937_64 &mt)
{
	uniform_int_distribution<unsigned char> act_dis(0, 1);
	uniform_int_distribution<int> resource_dis(0, (int)(resource_ids.size() - 1));
	auto res_1 = resources[resource_ids[resource_dis(mt)]];
	auto res_2 = resources[resource_ids[resource_dis(mt)]];
	uniform_int_distribution<unsigned char> unit_1_dis(0, (int)(res_1.units.size() - 1));
	uniform_int_distribution<unsigned char> unit_2_dis(0, (int)(res_2.units.size() - 1));
	if (act_dis(mt) == 0)
	{
		// swap random units
		auto unit_1_id = unit_1_dis(mt);
		auto unit_2_id = unit_2_dis(mt);
		auto temp = move(res_1.units[unit_1_id]);
		res_1.units[unit_1_id] = move(res_2.units[unit_2_id]);
		res_2.units[unit_2_id] = move(temp);
	}
	else
	{
		// move random unit to random machine
		res_2.push_back(move(res_1.units[unit_1_dis(mt)]));
	}
	return resources;
};

template<typename next_function>
unordered_map<int, resource> 
simulated_annealing(vector<int> &unit_ids, unordered_map<int, unit> &units,
	vector<int> &resource_ids, unordered_map<int, resource> &old_resources,
	double barrier_value, double cooling_rate, next_function& nf, mt19937_64 &mt)
{
	//timer<std::chrono::high_resolution_clock> t;

	auto start = std::chrono::system_clock::now();
	auto old_cost = ef(resource_ids, old_resources);

	auto best_resources = old_resources;
	auto best_cost = old_cost;

	std::uniform_real_distribution<float> rf(0, 1);
	double max_temperature = 1.0;
	double min_temperature = 0.00001;
	double temperature = max_temperature;
	
	while (temperature > min_temperature)
	{
		temperature *= cooling_rate;
		cout << "best: " << best_cost << "| temp: " << temperature << endl;
		//t.start();
		auto new_resources = nf(unit_ids, units, resource_ids, old_resources, mt);
		//t.finish();
		//cout << "1. Time elapsed: " << t.count<double>() << endl;

		//t.start();
		auto new_cost = ef(resource_ids, new_resources);
		//t.finish();
		//cout << "2. Time elapsed: " << t.count<double>() << endl;

		if (new_cost < old_cost)
		{
			best_resources = new_resources;
			best_cost = new_cost;
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
			//cout << "first catch: " << best_cost << endl;
			continue;
		}

		auto delta = new_cost - old_cost;
		auto delta_ratio = delta / old_cost;
		if (delta_ratio > barrier_value) continue;

		if (std::exp(-(delta_ratio/temperature)) > rf(mt))
		{
			cout << "hit" << endl;
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
		}

	}
	return best_resources;
}

int main()
{
	mt19937_64 mt(100);

	factory_utility factory = factory_utility::factory_utility();
	
	unordered_map<int, resource> resources;
	auto resource_ids = factory.generate_resources(resources);


	unordered_map<int, part> parts;
	auto part_ids = factory.generate_parts(parts, mt);

	unordered_map<int, order> orders;
	auto order_ids = factory.generate_orders(part_ids, orders, mt);

	unordered_map<int, unit> units;
	auto unit_ids = factory.split_orders_to_units(order_ids, orders, units, parts);

	resources = random_function(unit_ids, units, resource_ids, resources, mt);

	simulated_annealing(unit_ids, units, resource_ids, resources, 0.0, 0.9997, random_function, mt);
	simulated_annealing(unit_ids, units, resource_ids, resources, 0.2, 0.9997, next_function, mt);

	int end;
	cin >> end;
	return 1;
}

