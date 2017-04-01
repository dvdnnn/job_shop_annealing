#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

#define NUMBER_OF_PARTS 10
#define NUMBER_OF_ORDERS 1000
#define NUMBER_OF_MACHINES 4

#define UNIT_SIZE_MIN 0.1f
#define UNIT_SIZE_MAX 2.0f

#define RUNTIME_MIN 15
#define RUNTIME_MAX 60

#define BATCH_SIZE_MIN 1.0f
#define BATCH_SIZE_MAX 5.0f

#define QUANTITY_MIN 1.0f
#define QUANTITY_MAX 20.0f

// Time is relative to the start of the schedule
#define MAX_DUE_DATE 60*24*7*3

#define RANDOM_SEED 100

struct part
{
	short id;
	float unit_size;
	float batch_size;
	int batch_runtime_minutes;
	vector<unsigned char> possible_machines;
};

struct order
{
	short id;
	part &part;
	float quantity;
	int due_date;
};

struct unit
{
	short id;
	order &order;
};

struct machine
{
	short id;
	vector<unit *> units;
};

vector<machine> generate_machines(mt19937_64 &mt)
{
	vector<machine> machines;
	for (short i = 0; i < NUMBER_OF_MACHINES; ++i)
	{
		machines.push_back({
			i,
			vector<unit *>()
		});
	}
	return machines;
}

vector<part> generate_parts(mt19937_64 &mt)
{
	vector<part> parts;
	uniform_real_distribution<float> batch_dis(BATCH_SIZE_MIN, BATCH_SIZE_MAX);
	uniform_int_distribution<int> runtime_dis(RUNTIME_MIN, RUNTIME_MAX);
	uniform_real_distribution<float> unit_size_dis(UNIT_SIZE_MIN, UNIT_SIZE_MAX);
	uniform_int_distribution<int> possible_machine_dis(1, NUMBER_OF_MACHINES);

	vector<int> machine_indices({ 0, 1, 2, 3 });

	for (short i = 0; i < NUMBER_OF_PARTS; ++i)
	{
		float unit_size = unit_size_dis(mt);
		part part = {
			i,
			unit_size,
			batch_dis(mt) * unit_size,
			runtime_dis(mt)
		};
		char m_count = possible_machine_dis(mt);
		shuffle(machine_indices.begin(), machine_indices.end(), mt);
		for (char i = 0; i < m_count; ++i)
		{
			part.possible_machines.push_back(machine_indices[i]);
		}
		parts.push_back(part);
	}
	return parts;
}

vector<order> generate_orders(vector<part> &parts, mt19937_64 &mt)
{
	vector<order> orders;
	uniform_real_distribution<float> quantity_dis(QUANTITY_MIN, QUANTITY_MAX);
	uniform_int_distribution<int> due_time_dis(5000, MAX_DUE_DATE);
	uniform_int_distribution<int> part_dis(0, NUMBER_OF_PARTS - 1);
	for (short i = 0; i < NUMBER_OF_ORDERS; ++i)
	{
		orders.push_back({
			i,
			parts[part_dis(mt)],
			quantity_dis(mt),
			due_time_dis(mt),
		});
	}
	return orders;
}

void empty_schedule(vector<machine> &machines)
{
	for (size_t i = 0; i < machines.size(); ++i)
	{
		machines[i].units.clear();
	}
}

void randomize_schedule(vector<machine> &machines, vector<unit> &units, mt19937_64 &mt)
{
	empty_schedule(machines);

	vector<reference_wrapper<unit>> units_wrapper(units.begin(), units.end());

	std::shuffle(units_wrapper.begin(), units_wrapper.end(), mt);

	uniform_int_distribution<int> machine_dis(0, NUMBER_OF_MACHINES - 1);
	for (size_t i = 0; i < units.size(); ++i)
	{
		machines[units_wrapper[i].get().order.part.possible_machines[mt() % units_wrapper[i].get().order.part.possible_machines.size()]].units.push_back(&units_wrapper[i].get());
	}
}


vector<unit> split_orders_to_units(vector<order> &orders)
{
	vector<unit> units;
	short iter(0);
	for (short i = 0; i < NUMBER_OF_ORDERS; ++i)
	{
		for (short j = 0; j < (short)(orders[i].quantity / orders[i].part.unit_size); ++j)
		{
			units.push_back({
				iter++,
				orders[i]
			});
		}
	}
	return units;
}

int cost(vector<machine> &machines)
{
	unsigned int cost = 0;
	for (machine m : machines)
	{
		short time_used = 0;
		float alloted_available_batch = 0;
		short prev_part_id = (*m.units[0]).id;
		for (unit *u : m.units)
		{
			if (prev_part_id == (*u).order.part.id 
				&& alloted_available_batch > 0)
			{
				alloted_available_batch -= (*u).order.part.unit_size;
			}
			else
			{
				time_used += (*u).order.part.batch_runtime_minutes;
				alloted_available_batch = (*u).order.part.batch_size - (*u).order.part.unit_size;
			}
			short temp_cost = (time_used - (*u).order.due_date);
			cost += (temp_cost > 0 ? temp_cost : 0);
		}
	}
	return cost;
}
void anneal(vector<machine> &solution, vector<unit> &units, mt19937_64 &mt)
{
	int old_cost = cost(solution);
	float temperature = 1.0f;
	float temperature_min = 0.00001f;
	float alpha = 0.9f;

	uniform_real_distribution<float> dis(0, 1);
	while (temperature > temperature_min)
	{
		for (short i = 0; i < 100; ++i)
		{
			vector<machine> new_solution = generate_machines(mt);
			randomize_schedule(new_solution, units, mt);
			int new_cost = cost(new_solution);
			if (new_cost < old_cost || std::exp((old_cost - new_cost) / temperature) > dis(mt))
			{
				solution = new_solution;
				old_cost = new_cost;
				cout << old_cost << endl;
			}
		}
		temperature *= alpha;
	}
}

int main()
{
	mt19937_64 mt(RANDOM_SEED);
	auto start = chrono::high_resolution_clock::now();
	auto machines = generate_machines(mt);
	auto parts = generate_parts(mt);
	auto orders = generate_orders(parts, mt);
	auto units = split_orders_to_units(orders);
	auto end = chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	cout << diff.count() << " s" << endl;
	start = chrono::high_resolution_clock::now();
	randomize_schedule(machines, units, mt);
	for (int i = 0; i < 10000; ++i)
	{
		anneal(machines, units, mt);
	}
	end = chrono::high_resolution_clock::now();
	diff = end - start;
	cout << diff.count() << " s" << endl;
	int hello;
	cin >> hello;
}

