#include "factory_utility.h"

#include "timer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>


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

	uniform_int_distribution<int> res_1_dis(0, (int)(resource_ids.size() - 1));
	auto res_1 = &resources[resource_ids[res_1_dis(mt)]];

	uniform_int_distribution<unsigned char> unit_1_dis(0, (int)(res_1->units.size() - 1));

	for (short i = 0; i < 1000; ++i)
	{
		auto action = act_dis(mt);
		auto to_move = (res_1->units.begin() + unit_1_dis(mt));
		uniform_int_distribution<int> res_2_dis(0,
			(int)((*to_move)->get_part()->get_possible_machines().size() - 1));
		auto res_2 = &resources[(*to_move)->get_part()->get_possible_machines()[res_2_dis(mt)]];

		uniform_int_distribution<unsigned char> unit_2_dis(0, (int)(res_2->units.size() - 1));
		if (action == 0)
		{
			// swap random units
			auto unit_2_id = unit_2_dis(mt);

			auto temp = move(*to_move);
			(*to_move) = move(res_2->units[unit_2_id]);
			res_2->units[unit_2_id] = move(temp);
		}
		else
		{
			// move random unit to random machine
			res_2->units.push_back(move(*to_move));
			res_1->units.erase(to_move);
		}
	}
	return resources;
};

template<typename next_function>
unordered_map<int, resource> 
simulated_annealing(vector<int> &unit_ids, unordered_map<int, unit> &units,
	vector<int> &resource_ids, unordered_map<int, resource> &old_resources,
	double barrier_value, double cooling_rate, next_function& nf, mt19937_64 &mt)
{
	timer<std::chrono::high_resolution_clock> t;

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
		//t.start();
		
		temperature *= cooling_rate;
		auto new_resources = nf(unit_ids, units, resource_ids, old_resources, mt);

		auto new_cost = ef(resource_ids, new_resources);

		if (new_cost < old_cost)
		{
			best_resources = new_resources;
			best_cost = new_cost;
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
			continue;
		}

		auto delta = new_cost - old_cost;
		auto delta_ratio = delta / old_cost;

		if (delta_ratio > barrier_value) continue;

		if (std::exp(-(delta_ratio/temperature)) > rf(mt))
		{
			cout << delta_ratio << endl;
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
		}

		//t.finish();
		//cout << "1. Time elapsed: " << t.count<double>() << endl;
	}
	return best_resources;
}

void print_resources(vector<int> resource_ids, unordered_map<int, resource> resources)
{

//    "-------------------------"
//    "|  1  |  2  |  3  |  4  |"
//    "-------------------------"
//    "|  3  | 23  | 222 |  2  |"
//    "-------------------------"

	int max = 0;
	for (auto res_id : resource_ids)
	{
		if (resources[res_id].units.size() > max)
		{
			max = resources[res_id].units.size();
		}
	}

	cout << "-------------------------" << endl
		 << "|  1  |  2  |  3  |  4  |" << endl
		 << "-------------------------" << endl
		 << "-------------------------" << endl
		 << "-------------------------" << endl;

	for (int i = 0; i < max; ++i)
	{
		string row = "|";
		for (auto res_id : resource_ids)
		{
			if (i < resources[res_id].units.size())
			{
				row.append(2 - std::floor(resources[res_id].units[i]->get_part()->get_id() / 10), ' ');
				row.append(to_string(resources[res_id].units[i]->get_part()->get_id()));
				row.append(2 - std::ceil(resources[res_id].units[i]->get_part()->get_id() / 10), ' ');
			}
			else
			{
				row.append(5, ' ');
			}
			row.append("|");
		}
		cout << row << endl;
		cout << "-------------------------" << endl;
	}
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


	timer<std::chrono::high_resolution_clock> t;
	{
		t.start("Random step");

		//resources = simulated_annealing(unit_ids, units, resource_ids, resources, 0.0, 0.9997, random_function, mt);

		t.finish();
		cout << "1. Time elapsed: " << t.count<double>() << endl;
	}

	{
		t.start("Next step");

		resources = simulated_annealing(unit_ids, units, resource_ids, resources, 0.2, 0.9997, next_function, mt);

		t.finish();
		cout << "1. Time elapsed: " << t.count<double>() << endl;
	}

	cout << ef(resource_ids, resources) << endl;
	print_resources(resource_ids, resources);

	int end;
	cin >> end;
	return 1;
}

