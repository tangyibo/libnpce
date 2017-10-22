#ifndef __TIME_COUNTER_H__
#define __TIME_COUNTER_H__
#include <sys/time.h>
#include <sys/types.h>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace utility
{

class StdClockTimer
{
public:

	StdClockTimer()
	{
		_start_time = std::clock();
	}

	void restart()
	{
		_start_time = std::clock();
	}

	double elapsed() const
	{
		return double(std::clock() - _start_time) / CLOCKS_PER_SEC;
	}

private:
	std::clock_t _start_time;
};


class WallClockTimer
{

public:
	WallClockTimer()
	{
		_start_time = get_time();
	}

	void restart()
	{
		_start_time = get_time();
	}

	double elapsed() const
	{
		return get_time() - _start_time;
	}

	static double get_time()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec + (tv.tv_usec / 1000000.0);
	}

private:
	double _start_time;
};

} //  namespace utility

#endif
