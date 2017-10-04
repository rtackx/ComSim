#include "libs.h"

Save::Save(string directory, string prefix)
{
	this->directory = directory;
    this->prefix = prefix;

	struct stat st = {0};

	if(stat(directory.c_str(), &st) == -1)
	{
		if(mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
			perror("mkdir");
	}
}

string Save::get_path(string filename) const
{
	return directory + "/" + prefix + "_" + filename;
}

void display_duration(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2)
{
    auto duration = duration_cast<minutes>( t2 - t1 ).count();
    if(duration == 0)
    {
        duration = duration_cast<seconds>( t2 - t1 ).count();
        if(duration == 0)
        {
            duration = duration_cast<milliseconds>( t2 - t1 ).count();
            printf("\r%i ms\n", duration);
        }
        else
            printf("\r%i sec\n", duration);
    }
    else
        printf("\r%i min\n", duration);
}

void display_ms(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2)
{
    auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
    //printf("\r%i\n", duration);
    cout << duration << endl;
}

void display_nano(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2)
{
    auto duration = duration_cast<nanoseconds>( t2 - t1 ).count();
    //printf("\r%i\n", duration);
    cout << duration << endl;
}

int get_random_element(vector<unsigned int>& v)
{
    if(v.size() == 0)
        return -1;
    
    return v[rand() % v.size()];
}

vector<unsigned int> random_map_element(unordered_map<unsigned int, unsigned int> list_best_similar)
{
    vector<unsigned int> v;
    for(auto i: list_best_similar)
        v.push_back(i.first);

    random_shuffle(v.begin(), v.end());

    return v;
}

vector<unsigned int> sort_desc_map_element(unordered_map<unsigned int, unsigned int> list_best_similar)
{
    vector<unsigned int> v;
    for(auto i: list_best_similar)
        v.push_back(i.first);

    sort(v.begin(), v.end(), greater<int>());

    return v;
}

int get_last_element(vector<unsigned int> v)
{
    if(v.size() == 0)
        return -1;
    return v.back();
}

void display_peakRSS()
{
    struct rusage r_usage;
    getrusage(RUSAGE_SELF,&r_usage);
    cout <<  r_usage.ru_maxrss << endl;
    //cout << r_usage.ru_stime.tv_sec << "." << r_usage.ru_stime.tv_usec << endl;
}