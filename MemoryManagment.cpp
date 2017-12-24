#include "Header.h"

int main()
{
	vector<process> processes; //reciving all processes from input file then put it sorted in outputvector to print it
	deque <process> current; //the deque of RoundRobin => deque to access any needed element
	process check; //to check if it is not equal to current front so print queue list
	ifstream inFile("Processes.txt"); //input file
	if (inFile.is_open())
	{
		processes = read_process(inFile, Quantum, Switch); //return vector of all processes
	}
	else
	{
		cout << "Unable to open file";
	}
	int temp = Quantum; //used if process runtime > quantum
	bool Way = false; //to continue in the path of runtime > quantum
	bool Switched = false; //if there is switch so check arrived process in current time step and during switching time
	bool halting = false;
	bool noswitch = false;
	hole temphole;
	temphole.start = 0;
	temphole.end = 1023;
	temphole.oRe = 1;
	temphole.size = 1024;
	holes.push_front(temphole);

	while (output.size() < processes.size()) //run until the output vector equals the original processes vector
	{
		if (!Switched)
		{
			ArrivingProcess(processes, current,check, timestep);
		}
		else
		{
			ArrivingProcess(processes, current,check, timestep - 3);
			ArrivingProcess(processes, current,check, timestep - 2);
			ArrivingProcess(processes, current,check, timestep - 1);
			ArrivingProcess(processes, current,check, timestep);
			Switched = false;
		}

		if (current.size() > 0)
		{
			if (current.front().startedbefore == false) //allocate memory when a new process started wasne't started before
			{
				if (get_mem(current) == false) //no space currently avalible for this process
				{
					out << ",	halted at " << timestep << " where halt count = "<< current.front().haltcount << " , as there is no current space for it" << endl;
					if (current.front().haltcount == 5)
					{
						out << ",	Terminated at " << timestep << " , as there is no space for it" << endl;
						current.front().finish_time = timestep;
						output.push_back(current.front());
						current.pop_front();
					}
					else
					{
						process temphalt = current.front();
						current.pop_front();
						current.push_back(temphalt);
					}
					halting = true;
				}
				else
				{
					current.front().startedbefore = true;
					halting = false;
				}
			}
			else
			{
				halting = false;
			}
		}

		if (current.size() > 0 && halting == false)
		{
			if (current.front().run > Quantum || Way == true) 
			{
				Way = true;
				if (temp > 0) 
				{
					//decrement the temp (= Quantum) till it equal zero then subtract process remaining runtime by the original Quantum
					temp--;
				}
				else
				{
					current.front().run -= Quantum;
					temp = Quantum;
					Way = false;
					if (current.size() > 1) //if there is only 1 process in the memory no need to pop and push again
					{
						out << ",	stopped at " << timestep << ", " << current.front().mem_rem << " remaining, memory starts at " << current.front().mem_start << " and ends at " << current.front().mem_end << endl;
						process traverse = current.front();
						current.pop_front();
						current.push_back(traverse);
						timestep++;
						out << "Process switching	: started at " << timestep;
						timestep += 2;
						out << ",	finished at " << timestep << endl;
						Switched = true;
					}
				}
			}
			else //runtime < quantium
			{
				if (current.front().run > 0) 
				{
					current.front().run--;
				}
				else
				{
					current.front().finish_time = timestep;
					out << ",	finished at " << timestep << ", memory starts at " << current.front().mem_start << " and ends at " << current.front().mem_end << endl;
					
					//memory merging occurs when process finish only
					merg_mem(current);

					output.push_back(current.front()); //insert the process in the output vector to print them
					current.pop_front();
					temp = Quantum;
					if (current.size() > 0) 
					{
						//print only if there is reamining process after it
						timestep++;
						out << "Process switching	: started at " << timestep;
						timestep += 2;
						out << ",	finished at " << timestep << endl;
					}
					Switched = true;
				}
			}
		}
		//time step is incremented in every cycle
		timestep++;
	}
	out.close(); //close log file
	sort(output.begin(), output.end(), by_arrival()); //sort vector to output it
	PrintingOutput(output);
	//system("Pause");
	return 0;
}