#ifndef LIBS_H
#define LIBS_H

#define SIZE_BUFFER 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <list>
#include <stack>
#include <random>
#include <iterator>
#include <chrono>
#include <functional>

using namespace std;
using namespace std::chrono;

struct comp_pair
{
	inline bool operator()(const pair<unsigned int, float>& lhs, const pair<unsigned int, float>& rhs) const 
	{
		if(lhs.second > rhs.second)
			return true;
		if(lhs.second < rhs.second)
			return false;

		return true;

		// if(lhs.first < rhs.first)
		//     return true;
		// return false;
	}
};

#endif