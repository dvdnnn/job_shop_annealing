#include "factory_utility.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include "timer.h"


using namespace std;
using namespace capacity_planning;

//void empty_schedule(vector<resource> &resources)
//{
//	for (size_t i = 0; i < resources.size(); ++i)
//	{
//		resources[i].units.clear();
//	}
//}

//void randomize_schedule(vector<resource> &resources, vector<unit> &units, mt19937_64 &mt)
//{
//	//empty_schedule(resources);
//
//	vector<reference_wrapper<unit>> units_wrapper(units.begin(), units.end());
//
//	std::shuffle(units_wrapper.begin(), units_wrapper.end(), mt);
//
//	uniform_int_distribution<int> machine_dis(0, NUMBER_OF_MACHINES - 1);
//	for (size_t i = 0; i < units.size(); ++i)
//	{
//		resources[units_wrapper[i].get().order.part.possible_machines[mt() % units_wrapper[i].get().order.part.possible_machines.size()]].units.push_back(&units_wrapper[i].get());
//	}
//}



//int cost(vector<resource> &resources)
//{
//	unsigned int cost = 0;
//	for (resource m : resources)
//	{
//		short time_used = 0;
//		float alloted_available_batch = 0;
//		short prev_part_id = (*m.units[0]).id;
//		for (unit *u : m.units)
//		{
//			if (prev_part_id == (*u).order.part.id 
//				&& alloted_available_batch > 0)
//			{
//				alloted_available_batch -= (*u).order.part.unit_size;
//			}
//			else
//			{
//				time_used += (*u).order.part.batch_runtime_minutes;
//				alloted_available_batch = (*u).order.part.batch_size - (*u).order.part.unit_size;
//			}
//			short temp_cost = (time_used - (*u).order.due_date);
//			cost += (temp_cost > 0 ? temp_cost : 0);
//		}
//	}
//	return cost;
//}

//void anneal(vector<machine> &solution, vector<unit> &units, mt19937_64 &mt)
//{
//	int old_cost = cost(solution);
//	float temperature = 1.0f;
//	float temperature_min = 0.00001f;
//	float alpha = 0.9f;
//
//	uniform_real_distribution<float> dis(0, 1);
//	while (temperature > temperature_min)
//	{
//		for (short i = 0; i < 100; ++i)
//		{
//			vector<machine> new_solution = generate_machines(mt);
//			randomize_schedule(new_solution, units, mt);
//			int new_cost = cost(new_solution);
//			if (new_cost < old_cost || std::exp((old_cost - new_cost) / temperature) > dis(mt))
//			{
//				solution = new_solution;
//				old_cost = new_cost;
//				cout << old_cost << endl;
//			}
//		}
//		temperature *= alpha;
//	}
//}

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
				alloted_available_batch -= u->get_part()->get_unit_size();
			}
			else
			{
				time_used += u->get_part()->get_batch_runtime_minutes();
				alloted_available_batch = u->get_part()->get_batch_size() - u->get_part()->get_unit_size();
			}
			short temp_cost = (time_used - u->get_due_date());
			cost += (temp_cost > 0 ? temp_cost : 0);
		}
	}
	return std::abs((long)((cost - 10)*(cost + 20)*(cost - 30)));
};

auto tf = [](long double k)
{
	return std::exp(-20 * k);
};

unordered_map<int, resource> randomize(vector<int> unit_ids, unordered_map<int, unit> &units,
			   vector<int> resource_ids, mt19937_64 &mt)
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
			units[unit_ids[i]].get_part()->get_possible_machines().size() - 1);

		new_resources[resource_ids[resource_dis(mt)]].push_back(&units[unit_ids[i]]);
	}

	return new_resources;
}

unordered_map<int, resource> 
simulated_annealing(vector<int> &unit_ids, unordered_map<int, unit> &units,
	vector<int> &resource_ids, unordered_map<int, resource> &old_resources,
	size_t count,  mt19937_64 &mt)
{
	auto start = std::chrono::system_clock::now();
	auto old_cost = ef(resource_ids, old_resources);

	auto best_resources = old_resources;
	auto best_cost = old_cost;

	std::uniform_real_distribution<double long> rf(0, 1);

	for (; count > 0; --count)
	{
		auto new_resources = randomize(unit_ids, units, resource_ids, mt);
		auto new_cost = ef(resource_ids, new_resources);

		if (new_cost < old_cost)
		{
			best_resources = new_resources;
			best_cost = new_cost;
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
			cout << "first catch: " << best_cost << endl;
			continue;
		}

		auto t = tf(count);
		auto delta_e = new_cost - old_cost;

		if (delta_e > 10.0 * t) continue;

		if (delta_e <= 0.0 || std::exp(-delta_e / t) > rf(mt))
		{
			old_resources = std::move(new_resources);
			old_cost = std::move(new_cost);
			cout << "second catch: " << best_cost << endl;
		}
	}
	return(best_resources);
}

int main()
{
	mt19937_64 mt(100);
	//auto start = chrono::high_resolution_clock::now();
	factory_utility factory = factory_utility::factory_utility();
	
	unordered_map<int, resource> resources;
	auto resource_ids = factory.generate_resources(resources);


	unordered_map<int, part> parts;
	auto part_ids = factory.generate_parts(parts, mt);

	unordered_map<int, order> orders;
	auto order_ids = factory.generate_orders(part_ids, orders, mt);

	unordered_map<int, unit> units;
	auto unit_ids = factory.split_orders_to_units(order_ids, orders, units, parts);

	//auto end = chrono::high_resolution_clock::now();
	//std::chrono::duration<double> diff = end - start;
	//cout << diff.count() << " s" << endl;

	timer<std::chrono::high_resolution_clock> t;

	t.start();
	resources = randomize(unit_ids, units, resource_ids, mt);
	t.finish();

	std::cout << "Time elapaed: " << t.count<double>() << '\n';

	t.start();
	ef(resource_ids, resources);
	t.finish();
	std::cout << "Time elapaed: " << t.count<double>() << '\n';

	simulated_annealing(unit_ids, units, resource_ids, resources, 1000, mt);
	//std::random_device rd;
	//std::mt19937_64 g(rd());
	//timer<std::chrono::high_resolution_clock> t;

	//long double root = 1000;//std::atof(argv[1]);
	//std::size_t count = 1000;//std::atoi(argv[2]);

	//t.start();
	////root = simulated_annealing(root, count, cost, tf, randomize_schedule<decltype(g)>, g);
	//t.finish();
	//cin >> hello;
	return 1;
}

