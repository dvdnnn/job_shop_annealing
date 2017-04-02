#include <cmath>
#include <random>

#include <chrono>
#include <cstdlib>
#include <iostream>

#include "simulated_annealing.h"

template<typename Clock = std::chrono::system_clock>
class timer
{
private:
	typedef std::chrono::time_point<Clock> time_point;
	time_point _start;
	time_point _finish;
public:
	void start()
	{
		_start = Clock::now();
	}
	void finish()
	{
		_finish = Clock::now();
	}
	template<typename rtype>
	rtype count()
	{
		std::chrono::duration<rtype> elapsed = _finish - _start;
		return elapsed.count();
	}
};


//auto ef = [](long double x)
//{
//	return std::abs((x - 10)*(x + 20)*(x - 30));
//};

//auto tf = [](long double k)
//{
//	return std::exp(-20 * k);
//};

//template<typename generator>
//long double nf(long double x, generator& g)
//{
//	std::normal_distribution<decltype(x)> d(0, 1);
//	return x + d(g);
//}

int maain(int argc, char const *argv[])
{
	std::random_device rd;
	std::mt19937_64 g(rd());
	timer<std::chrono::high_resolution_clock> t;

	long double root = 1000;//std::atof(argv[1]);
	std::size_t count = 1000;//std::atoi(argv[2]);

	t.start();
	//root = simulated_annealing(root, count, ef, tf, nf<decltype(g)>, g);
	t.finish();

	std::cout << "Result: " << root << std::endl;
	std::cout << "Time elapaed: " << t.count<double>() << '\n';

	return 0;
}