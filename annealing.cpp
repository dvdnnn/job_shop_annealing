#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

#define NUMBER_OF_PARTS 10
#define NUMBER_OF_ORDERS 1000
#define NUMBER_OF_MACHINES 4

#define UNIT_SIZE_MIN 0.1f
#define UNIT_SIZE_MAX 2.0f

#define RUNTIME_MIN 15.0f
#define RUNTIME_MAX 60.0f

#define BATCH_SIZE_MIN 1.0f
#define BATCH_SIZE_MAX 5.0f

#define QUANTITY_MIN 1.0f
#define QUANTITY_MAX 20.0f

// Time is relative to the start of the schedule
#define MAX_DUE_DATE 5000

#define RANDOM_SEED 100

struct part
{
	short id;
	float unit_size;
	float batch_size;
	float batch_runtime_minutes;
	bool possible_machines[NUMBER_OF_MACHINES];
};

struct order
{
	short id;
	part &part;
	float quantity;
	float due_date;
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

vector<part> generate_parts(mt19937_64 mt)
{
	vector<part> parts;
	uniform_real_distribution<float> batch_dis(BATCH_SIZE_MIN, BATCH_SIZE_MAX);
	uniform_real_distribution<float> runtime_dis(RUNTIME_MIN, RUNTIME_MAX);
	uniform_real_distribution<float> unit_size_dis(UNIT_SIZE_MIN, UNIT_SIZE_MAX);
	uniform_int_distribution<int> possible_machine_dis(0, 1);
	for (short i = 0; i < NUMBER_OF_PARTS; ++i)
	{
		float unit_size = unit_size_dis(mt);
		part part = {
			i,
			unit_size,
			batch_dis(mt) * unit_size,
			runtime_dis(mt)
		};
		for (short i = 0; i < NUMBER_OF_MACHINES; ++i)
		{
			part.possible_machines[i] = possible_machine_dis(mt);
		}
		parts.push_back(part);
	}
	return parts;
}

vector<order> generate_orders(vector<part> &parts, mt19937_64 mt)
{
	vector<order> orders;
	uniform_real_distribution<float> quantity_dis(QUANTITY_MIN, QUANTITY_MAX);
	uniform_real_distribution<float> due_time_dis(100, MAX_DUE_DATE);
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

void randomize_schedule(vector<machine> &machines, vector<unit> &units, mt19937_64 mt)
{
	machines.clear();
	uniform_int_distribution<int> machine_dis(0, NUMBER_OF_MACHINES - 1);
	for (int i = 0; i < units.size(); ++i)
	{
		machines[machine_dis(mt)].units.push_back(&units[i]);
	}
}

void anneal(vector<machine> &machines)
{

}

vector<unit> split_orders_to_units(vector<order> &orders)
{
	vector<unit> units;
	for (short i = 0; i < NUMBER_OF_ORDERS; ++i)
	{
		for (int j = 0; j < (int)(orders[i].quantity / orders[i].part.unit_size); ++j)
		{
			units.push_back({
				i,
				orders[i]
			});
		}
	}
	return units;
}

int main()
{
	mt19937_64 mt(RANDOM_SEED);
	auto start = chrono::high_resolution_clock::now();
	auto parts = generate_parts(mt);
	auto orders = generate_orders(parts, mt);
	auto units = split_orders_to_units(orders);
	auto end = chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	cout << diff.count() << " s" << endl;
	int hello;
	cin >> hello;
}

