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

using namespace std;
using namespace std::chrono;

class Save
{
public:
	string directory;
	string prefix;
	Save(string, string);
	string get_path(string) const;
};

vector<unsigned int> random_map_element(unordered_map<unsigned int, unsigned int>);
vector<unsigned int> sort_desc_map_element(unordered_map<unsigned int, unsigned int>);
//int get_random_element_set(set<unsigned int>);
int get_last_element(vector<unsigned int>);
int get_random_element(vector<unsigned int>&);
void display_duration(high_resolution_clock::time_point, high_resolution_clock::time_point);
void display_ms(high_resolution_clock::time_point, high_resolution_clock::time_point);
void display_nano(high_resolution_clock::time_point, high_resolution_clock::time_point);
void display_peakRSS();

#endif