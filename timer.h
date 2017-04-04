#pragma once

#include <chrono>
#include <string>
#include <iostream>

template<typename Clock = std::chrono::system_clock>
class timer
{
private:
	typedef std::chrono::time_point<Clock> time_point;
	string _name;
	time_point _start;
	time_point _finish;
public:
	void start(string name)
	{
		_start = Clock::now();
		_name = name;
	}
	void finish()
	{
		_finish = Clock::now();
		std::cout << _name << ": " << count<double>();
	}
	template<typename rtype>
	rtype count()
	{
		std::chrono::duration<rtype> elapsed = _finish - _start;
		return elapsed.count();
	}
};