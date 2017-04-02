#pragma once

#include "resource.h"
#include "part.h"
#include "order.h"
#include "unit.h"

#include <unordered_map>
#include <random>
#include <time.h>

using namespace std;

namespace capacity_planning
{
	class factory_utility
	{
	public:
		factory_utility();
		~factory_utility();

		vector<int> generate_resources(unordered_map<int, resource> &resources);

		vector<int> generate_parts(unordered_map<int, part> &parts,
									mt19937_64 &mt);

		vector<int> generate_orders(vector<int> &part_ids,
									 unordered_map<int, order> &orders,
									 mt19937_64 &mt);

		vector<int> split_orders_to_units(vector<int> &order_ids,
										  unordered_map<int, order> &orders,
										  unordered_map<int, unit> &units,
										  unordered_map<int, part> &parts);

	private:
		const short _NUMBER_OF_PARTS = 10;
		const short _NUMBER_OF_ORDERS = 1000;
		const short _NUMBER_OF_RESOURCES = 4;

		const float _UNIT_SIZE_MIN = 0.1f;
		const float _UNIT_SIZE_MAX = 2.0f;

		const short _RUNTIME_MIN = 15;
		const short _RUNTIME_MAX = 60;

		const short _BATCH_SIZE_MIN = 1;
		const short _BATCH_SIZE_MAX = 5;

		const float _QUANTITY_MIN = 1.0f;
		const float _QUANTITY_MAX = 20.0f;

		// Time is relative to the start of the schedule
		#define MAX_DUE_DATE 60*24*7*3
		const int _MAX_DUE_DATE = MAX_DUE_DATE;
	};

}
