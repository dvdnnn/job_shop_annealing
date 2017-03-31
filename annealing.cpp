#include <vector>
#include <random>
#include <iostream>
#include <chrono>

using namespace std;

#define NUMBER_OF_PARTS 10
#define NUMBER_OF_ORDERS 1000
#define NUMBER_OF_MACHINES 4

#define RUNTIME_MIN 15.0f
#define RUNTIME_MAX 60.0f

#define BATCH_SIZE_MIN 1.0f
#define BATCH_SIZE_MAX 5.0f

#define QUANTITY_MIN 1.0f
#define QUANTITY_MAX 20.0f

#define RANDOM_SEED 100

struct part
{
	short id;
	float batch_size;
	float batch_runtime_minutes;
	vector<machine *> possible_machines;
};

struct order
{
	short id;
	part &prt;
	float quantity;
};

struct machine
{
	short id;
	vector<order *> parts;
};

vector<part> generate_parts()
{
	vector<part> parts;
	mt19937 gen(RANDOM_SEED);
	uniform_real_distribution<float> batch_dis(BATCH_SIZE_MIN, BATCH_SIZE_MAX);
	uniform_real_distribution<float> runtime_dis(RUNTIME_MIN, RUNTIME_MAX);
	for (short i = 0; i < NUMBER_OF_PARTS; ++i)
	{
		parts.push_back({
			i,
			batch_dis(gen),
			runtime_dis(gen)
		});
	}
	return parts;
}

vector<order> generate_orders(vector<part> &parts)
{
	vector<order> orders;
	mt19937 gen(RANDOM_SEED);
	uniform_real_distribution<float> quantity_dis(QUANTITY_MIN, QUANTITY_MAX);
	uniform_int_distribution<int> part_dis(0, NUMBER_OF_PARTS - 1);
	for (short i = 0; i < NUMBER_OF_ORDERS; ++i)
	{
		orders.push_back({
			i,
			parts[part_dis(gen)],
			quantity_dis(gen)
		});
	}
	return orders;
}

void randomize_schedule(vector<machine> &machines, vector<order> &orders)
{
	
}

void anneal(vector<machine> &machines)
{

}

int main()
{
	auto start = chrono::high_resolution_clock::now();
	auto parts = generate_parts();
	auto orders = generate_orders(parts);
	auto end = chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	cout << diff.count() << " s" << endl;
	int hello;
	cin >> hello;
}

