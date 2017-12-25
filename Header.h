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
#include <list>
using namespace std;
std::ofstream out("log.txt", std::ofstream::out); //out << to out in the log file
int Quantum = 0;
int Switch = 0;
int timestep = 0;

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
	int haltcount;
	bool startedbefore;
	process(string id , int runtime , int arrival , int size) :ID(id) , run_time(runtime) , run(runtime) , arrival_time(arrival) , finish_time(0) , mem_size(size) , mem_start(0) , mem_end(0) , mem_rem(0) , haltcount(0) , startedbefore(false){}
	process() {}
};

vector <process> output;

struct hole
{
	int start, end, size;
	int oRe;
	string pro_id;
};

list <hole> holes;
list <hole> mem;

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

void ArrivingProcess(vector<process> pro, deque <process> &curr, process &check, int time) //check arriced process in a certain time step
{
	for (int i = 0; i < pro.size(); i++)
	{
		if (pro[i].arrival_time == time) //at the timestep push the process in the deque
		{
			curr.push_back(pro[i]);
		}
	}
	if (check.ID != curr.front().ID) //to print the queue of the process in each timestep when front process changes
	{
		out << "Queue: ";
		for (int i = 0; i < curr.size(); i++)
		{
			out << curr[i].ID;
			if (i != curr.size() - 1)
			{
				out << ", ";
			}
		}
		check = curr.front();
		out << endl << "Executing process " << curr.front().ID << "	: started at " << timestep;
	}
}

bool get_mem(deque <process> &curr)    // Allocate memory for process
{
	string ID = curr.front().ID;
	int space = curr.front().mem_size + 6;
	int s;
	int pos;
	if (space > 1024 || space <= 0)       // if memory is outside  the range increment the haltcount
	{
		curr.front().haltcount ++;
		return false;
	}
	
	for (pos = 1; pos < 11; pos++)  // get the most suitable size to the process
	{
		if (space == powf(2, pos)) 
		{
			s = powf(2, pos);
			break;
		}
		else if((powf(2, pos) < space) && (powf(2, pos + 1) > space)) 
		{
			s = powf(2, pos + 1);
			break;
		}
	}
	int tempsize = 1024;
	hole temp;
	bool flag = false;
	std::list<hole>::iterator holepointer;
	std::list<hole>::iterator iterator = holes.begin();
	for (int i = 0; i< holes.size(); i++)   /// loop over the holes list to get the most suitable hole
	{
		if (s <= (*iterator).size && (*iterator).size <= tempsize)  
		{
			temp = *iterator;
			holepointer = iterator;
			tempsize = (*iterator).size;
			flag = true;
		}
		iterator++;
	}
	if (!flag)            // if there is no suitable place increase the haltcount
	{
		curr.front().haltcount++;
		return flag;
	}
	for (int i = 0; i<10; i++)  // divide the hole until its very near the process's memory
	{
		if (s >((temp).size / 2))   //if  need memory isreached and we don't need to divide any more allocate it
		{
			temp.pro_id = ID;
			mem.emplace_back(temp);
			curr.front().mem_start = temp.start;
			curr.front().mem_end = temp.end;
			curr.front().mem_rem = temp.size - space;
			holes.erase(holepointer);
			return true;
		}
		else   // divide the memory and update the holes list
		{
			hole t1;
			t1.size = ((temp).size) / 2;
			t1.start = (temp).start;
			t1.oRe = 1;
			t1.end = (temp).end - t1.size;
			holes.insert(holepointer, t1);
			iterator = --holepointer;
			holepointer++;
			t1.start = t1.end + 1;
			t1.end = (temp).end;
			t1.oRe = 2;
			holes.insert(holepointer, t1);
			holes.erase(holepointer);
			temp = *iterator;
			holepointer = iterator;
		}
	}
	curr.front().haltcount++;
	return false;
}

void merg_mem(deque <process> &curr)
{
	string id = curr.front().ID;  // get the id of the process to 
	std::list<hole>::iterator iterator = mem.begin();
	std::list<hole>::iterator it = holes.begin();
	hole temp;
	bool merg = true;
	bool first_insert = true;

	for (int i = 0; i < mem.size(); i++) 
	{
		if ((*iterator).pro_id == id)  // get pointer tp the memory at the memory list
		{
			break;
		}
		iterator++;
	}
    bool no_pair;

	while (merg) // loop to merge larger holes
	{
		bool first = true;
		merg = false;
		no_pair=true;
		
		if (holes.size() == 0 || (holes.size() == 1 && !first_insert))  
			break;
		it = holes.begin();

		for (int i = 0; i < holes.size(); i++)  // loop on all holes list
		{ 
			if (!first)  // if first time don not increment the it "iterator to holes "
			{
				it++;
			}
			first = false;

			if ((*it).start == (*iterator).start) //  if iterator and it points to the same element 
				continue;

			if ((*it).size == (*iterator).size)  //  if same size check if after or before
			{
				if ((*it).end + 1 == (*iterator).start && (*iterator).oRe == 2)  // check if iterator is the second match to the it 
				{
					temp.start = (*it).start;
					temp.end = (*iterator).end;
					no_pair=false;
				}

				else if ((*it).start == (*iterator).end + 1 && (*iterator).oRe == 1)  // chech if iterator is the first match to it 
				{
					temp.start = (*iterator).start;
					temp.end = (*it).end;
					no_pair=false;
				}

				temp.size = (*iterator).size * 2;                // double the size of the new element 
				int odd_even = (temp.end + 1) / temp.size;

				if (temp.start == 0 || odd_even % 2 == 1) 
				{
					temp.oRe = 1;
				}

				else 
				{
					temp.oRe = 2;
				}
				if(! no_pair){            // if there is a pair for the element  merg  the two pairs
				if (first_insert)
					mem.erase(iterator);
				holes.push_back(temp);
				holes.erase(it);
				if (!first_insert)
				holes.erase(iterator);
				std::list<hole>::iterator iter = holes.begin();
				advance(iter, holes.size() - 1);
				iterator = iter;
				merg = true;
				first_insert = false;
				break;
			}
			}
			else 
			{
				merg = false;
			}
		}

		if (!merg && first_insert && no_pair)  /// if it's first time to insert and no pairs 
		{
			holes.push_back(*iterator);
			mem.erase(iterator);
			std::list<hole>::iterator iter = holes.begin();
			advance(iter, holes.size() - 1);
			iterator = iter;
			first_insert = false;
			merg = true;
		}
	}
}

void PrintingOutput(vector<process> Output)
{
	//Output file
	FILE *fp = freopen("output.txt", "w", stdout);
	cout << "Quantum " << Quantum << endl;
	cout << "Switch " << Switch << endl;
	cout << "process_id	" << "run_time	" << "arrival_time	" << "finish_time		" << "mem_size	" << "mem_start	" << "mem_end" << endl;
	for (int i = 0; i < Output.size(); i++)
	{
		cout << Output[i].ID << "			" << Output[i].run_time << "			" << Output[i].arrival_time << "				" << Output[i].finish_time << "				";
		cout << Output[i].mem_size << "			" << Output[i].mem_start << "			" << Output[i].mem_end << endl;
	}
	fclose(fp);
}