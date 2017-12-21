#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <deque>
#include <cmath>
#include <iomanip>
using namespace std;

// process
struct process
{
	string ID;
	int run_time; 
	int arrival_time;
	int finish_time;
	int mem_size;
	int mem_start;
	int mem_end;
	int mem_rem; //remaining memorry
	int run; //remamining running time of the process
	process(string id , int runtime , int arrival , int size) :ID(id) , run_time(runtime) , run(runtime) , arrival_time(arrival) , finish_time(0) , mem_size(size) , mem_start(0) , mem_end(0) , mem_rem(0) {}
	process() {}
};

vector<process> read_process(istream &is , int &Quantumn , int &Switch) //reading the input file
{
	vector<process> result;
	string word;
	string ID;
	int arrival, run, size;

	is >> word;
	is >> Quantumn;
	is >> word;
	is >> Switch;
	is >> word >> word >> word >> word;

	while (is >> word)
	{
		ID = word;
		is >> run >> arrival >> size;
		result.emplace_back(ID, run, arrival, size); //put it in vector
	}
	return result;
}

struct by_arrival  //sorting by run time for the output file
{
	bool operator()(process const &a, process const &b) const
	{
		return a.arrival_time < b.arrival_time;
	}
};

